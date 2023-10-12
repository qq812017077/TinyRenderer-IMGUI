#include "Layout.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

namespace TinyEngine::Rendering
{
    struct ExtraData
    {
        struct Struct : public LayoutElement::ExtraDataBase
        {
            std::vector<std::pair<std::string,LayoutElement>> layoutElements;
        };

        struct Array : public LayoutElement::ExtraDataBase
        {
            std::optional<LayoutElement> layoutElement;
            size_t size;
        };
    };

/**************************************************************************************************************/
    std::string LayoutElement::GetSignature() const noexcept
    {
        switch( type )
		{
            #define X(el) case Type::el: return Map<Type::el>::code;
            LEAF_ELEMENT_TYPES
            #undef X
            case Type::Struct:
                return GetSignatureForStruct();
            case Type::Array:
                return GetSignatureForArray();
            default:
                assert( "Bad type in signature generation" && false );
                return "???";
		}
    }
    bool LayoutElement::Exists() const noexcept
    {
        return type != Type::Empty;
    }
    std::pair<size_t, const LayoutElement*> LayoutElement::CalculateIndexingOffset(size_t offset, size_t index) const noexcept
    {
        assert( "Indexing into non-array type" && type == Type::Array );
        const auto& data = static_cast<const ExtraData::Array&>(*pExtraData);
        assert( "Index out of bounds" && index < data.size );
        return { offset + data.layoutElement->GetSizeInBytes() * index, &*data.layoutElement };
    }
    
    LayoutElement& LayoutElement::operator[](const std::string& key) noexcept
    {
        assert( "Tried to index non-struct type" && type == Type::Struct );
        for( auto& e : static_cast<ExtraData::Struct&>(*pExtraData).layoutElements )
        {
            if( e.first == key )
            {
                return e.second;
            }
        }
        return GetEmptyElement();
    }
    const LayoutElement& LayoutElement::operator[](const std::string& key) const noexcept
    {
        return const_cast<LayoutElement&>( *this )[key]; 
    }
    // T() only works for Arrays; gets the array type layout object
    // needed to further configure an array's type
    LayoutElement& LayoutElement::T() 
    {
        assert( "Accessing T of non-array" && type == Type::Array );
		return *static_cast<ExtraData::Array&>(*pExtraData).layoutElement;
    }
    const LayoutElement& LayoutElement::T() const 
    {
        return const_cast<LayoutElement&>(*this).T();
    }
    // offset based- functions only work after finalization!
    size_t LayoutElement::GetOffsetBegin() const
    {
        return *offset;
    } 
    size_t LayoutElement::GetOffsetEnd() const 
    {
        switch (type)
        {
            #define X(el) case Type::el: return *offset + Map<Type::el>::hlslSize;
            LEAF_ELEMENT_TYPES
            #undef X
            case Type::Struct:
            {
                const auto& data = static_cast<const ExtraData::Struct&>(*pExtraData);
                return AdvanceToBoundary( *offset + data.layoutElements.back().second.GetOffsetEnd() );
            }
            case Type::Array:
            {
                const auto& data = static_cast<const ExtraData::Array&>(*pExtraData);
                return *offset + AdvanceToBoundary(data.layoutElement->GetSizeInBytes() ) * data.size;
            }
            default:
                assert( "Tried to get offset of non-leaf element" && false );
                return 0u;
        }
    }
    // get size in bytes derived from offsets
    size_t LayoutElement::GetSizeInBytes() const
    {
        return GetOffsetEnd() - GetOffsetBegin();
    }

    // only works for Structs; add LayoutElement to struct
    LayoutElement& LayoutElement::Add(Type addedType,std::string name )
    {
        assert( "Tried to add element to non-struct type" && type == Type::Struct );
        assert( "invalid symbol name in Struct" && ValidateSymbolName( name ) );
        auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
        for( auto & mem : data.layoutElements )
        {
            if(mem.first == name)
            {
                assert( "Tried to add duplicate symbol name to Struct" && false );
            }
        }

        data.layoutElements.emplace_back(std::move( name ), LayoutElement{addedType});
        return *this;
    }
    // only works for Arrays; set the type and the # of elements
    LayoutElement& LayoutElement::Set(Type addedType,size_t size )
    {
        assert( "Tried to set type of non-array type" && type == Type::Array );
        assert( "Tried to set size of array to 0" && size != 0u );
        auto& data = static_cast<ExtraData::Array&>(*pExtraData);
        data.layoutElement = LayoutElement{addedType};
        data.size = size;
        return *this;
    }

    /*****************************************************************************/
    /*                            PRIVATE FUNCTIONS                              */
    /*****************************************************************************/
    LayoutElement::LayoutElement( Type typeIn ) : type{ typeIn }
	{
		assert( typeIn != Type::Empty );
		if( typeIn == Type::Struct )
		{
			pExtraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
		}
		else if( typeIn == Type::Array )
		{
			pExtraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
		}
	}
    size_t LayoutElement::Finalize( size_t offsetIn )
	{
		switch( type )
		{
		#define X(el)   \
        case Type::el:  \
                    offset = AdvanceIfCrossesBoundary( offsetIn,Map<Type::el>::hlslSize );   \
                    return *offset + Map<Type::el>::hlslSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case Type::Struct:
			return FinalizeForStruct( offsetIn );
		case Type::Array:
			return FinalizeForArray( offsetIn );
		default:
			assert( "Bad type in size computation" && false );
			return 0u;
		}
	}
    std::string LayoutElement::GetSignatureForStruct() const 
	{
		using namespace std::string_literals;
		auto sig = "St{"s;
		for( const auto& el : static_cast<ExtraData::Struct&>(*pExtraData).layoutElements )
		{
			sig += el.first + ":"s + el.second.GetSignature() + ";"s;
		}
		sig += "}"s;
		return sig;
	}
    
    std::string LayoutElement::GetSignatureForArray() const
	{
		using namespace std::string_literals;
		const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		return "Ar:"s + std::to_string( data.size ) + "{"s + data.layoutElement->GetSignature() + "}"s;
	}

    size_t LayoutElement::FinalizeForStruct( size_t offsetIn )
	{
		auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
		assert( data.layoutElements.size() != 0u );
		offset = AdvanceToBoundary( offsetIn );
		auto offsetNext = *offset;
		for( auto& el : data.layoutElements )
		{
			offsetNext = el.second.Finalize( offsetNext );
		}
		return offsetNext;
	}
	size_t LayoutElement::FinalizeForArray( size_t offsetIn )
	{
		auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		assert( data.size != 0u );
		offset = AdvanceToBoundary( offsetIn );
		data.layoutElement->Finalize( *offset );
		// data.element_size = LayoutElement::AdvanceToBoundary( data.layoutElement->GetSizeInBytes() );
		return GetOffsetEnd();
	}
    
    // return true if a memory block crosses a boundary
    bool LayoutElement::CrossesBoundary( size_t offset,size_t size ) noexcept
    {
        const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
    }

    // supplement to 16 bytes
    size_t LayoutElement::AdvanceToBoundary( size_t offset ) noexcept
    {
        return offset + (16u - offset % 16u) % 16u;
    }

    // advance an offset to next boundary if block crosses a boundary
    size_t LayoutElement::AdvanceIfCrossesBoundary( size_t offset,size_t size ) noexcept
	{
		return CrossesBoundary( offset,size ) ? AdvanceToBoundary( offset ) : offset;
	}
    // check string for validity as a struct key
    bool LayoutElement::ValidateSymbolName( const std::string& name ) noexcept
    {
        // symbols can contain alphanumeric and underscore, must not start with digit
		return !name.empty() && !std::isdigit( name.front() ) &&
			std::all_of( name.begin(),name.end(),[]( char c ) {
				return std::isalnum( c ) || c == '_';
			}
		);
    }


/**************************************************************************************************************/

    Layout::Layout( std::shared_ptr<LayoutElement> pRoot ) noexcept
		:
		pRoot{ std::move( pRoot ) }
	{}
	size_t Layout::GetSizeInBytes() const noexcept
	{
		return pRoot->GetSizeInBytes();
	}
	std::string Layout::GetSignature() const noexcept
	{
		return pRoot->GetSignature();
	}

/**************************************************************************************************************/
	RawLayout::RawLayout() noexcept
		:
		Layout{ std::shared_ptr<LayoutElement>{ new LayoutElement(Type::Struct) } }
	{}
	LayoutElement& RawLayout::operator[](const std::string& key)
	{
		return (*pRoot)[key];
	}
	std::shared_ptr<LayoutElement> RawLayout::DeliverRoot() noexcept
	{
		auto temp = std::move( pRoot );
		temp->Finalize( 0 );
		*this = RawLayout();
		return std::move( temp );
	}
	void RawLayout::ClearRoot() noexcept
	{
		*this = RawLayout();
	}

/**************************************************************************************************************/
	CookedLayout::CookedLayout( std::shared_ptr<LayoutElement> pRoot ) noexcept
		:
		Layout( std::move( pRoot ) )
	{}
	std::shared_ptr<LayoutElement> CookedLayout::RelinquishRoot() const noexcept
	{
		return std::move( pRoot );
	}
	std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept
	{
		return pRoot;
	}
	const LayoutElement& CookedLayout::operator[](const std::string& key ) const
	{
		return (*pRoot)[key];
	}


/**************************************************************************************************************/
	bool ConstElementRef::Exists() const noexcept
	{
		return pLayout->Exists();
	}
	ConstElementRef ConstElementRef::operator[]( const std::string& key ) const
	{
		return { &(*pLayout)[key],pBytes,offset };
	}
	ConstElementRef ConstElementRef::operator[]( size_t index ) const
	{
		const auto indexingData = pLayout->CalculateIndexingOffset( offset,index );
		return { indexingData.second,pBytes,indexingData.first };
	}
	ConstElementRef::Ptr ConstElementRef::operator&() const
	{
		return Ptr{ this };
	}
	ConstElementRef::ConstElementRef( const LayoutElement* pLayout,const char* pBytes, size_t offset ) noexcept:
		offset( offset ),
		pLayout( pLayout ),
		pBytes( pBytes )
	{}
	ConstElementRef::Ptr::Ptr( const ConstElementRef* ref ) noexcept : ref( ref )
	{}
	
/**************************************************************************************************************/
	ElementRef::operator ConstElementRef() const noexcept
	{
		return { pLayout,pBytes,offset };
	}
	bool ElementRef::Exists() const noexcept
	{
		return pLayout->Exists();
	}
	ElementRef ElementRef::operator[]( const std::string& key ) const
	{
		return { &(*pLayout)[key],pBytes,offset };
	}
	ElementRef ElementRef::operator[]( size_t index ) const
	{
		const auto indexingData = pLayout->CalculateIndexingOffset( offset,index );
		return { indexingData.second,pBytes,indexingData.first };
	}
	ElementRef::Ptr ElementRef::operator&() const
	{
		return Ptr{ const_cast<ElementRef*>(this) };
	}
	ElementRef::ElementRef( const LayoutElement* pLayout,char* pBytes,size_t offset ) noexcept
		:
		offset( offset ),
		pLayout( pLayout ),
		pBytes( pBytes )
	{}
	ElementRef::Ptr::Ptr( ElementRef* ref ) noexcept : ref( ref )
	{}


    CookedLayout LayoutCodex::Resolve(RawLayout&& layout )
	{
		auto sig = layout.GetSignature();
		auto& map = Get_().map;
		const auto i = map.find( sig );
		// idential layout already exists
		if( i != map.end() )
		{
			// input layout is expected to be cleared after Resolve
			// so just throw away the layout tree
			layout.ClearRoot();
			return {i->second };
		}
		// otherwise add layout root element to map
		auto result = map.insert( { std::move( sig ),layout.DeliverRoot() } );
		// return layout with additional reference to root
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get_() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}