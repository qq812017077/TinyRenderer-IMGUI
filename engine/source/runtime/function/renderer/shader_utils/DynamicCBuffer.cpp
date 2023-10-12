#include "DynamicCBuffer.h"

namespace TinyEngine::Rendering
{
    DynamicBuffer::DynamicBuffer( RawLayout&& lay ):
		DynamicBuffer( LayoutCodex::Resolve( std::move( lay ) ) )
	{}
	DynamicBuffer::DynamicBuffer( const CookedLayout& lay )
		:
		pLayoutRoot( lay.ShareRoot() ),
		bytes( pLayoutRoot->GetOffsetEnd() )
	{}
	DynamicBuffer::DynamicBuffer( CookedLayout&& lay )
		:
		pLayoutRoot( lay.RelinquishRoot() ),
		bytes( pLayoutRoot->GetOffsetEnd() )
	{}
	DynamicBuffer::DynamicBuffer( const DynamicBuffer& buf ) noexcept
		:
		pLayoutRoot( buf.pLayoutRoot ),
		bytes( buf.bytes )
	{}
	DynamicBuffer::DynamicBuffer( DynamicBuffer&& buf ) noexcept:
		pLayoutRoot( std::move( buf.pLayoutRoot ) ),
		bytes( std::move( buf.bytes ) )
	{}
	ElementRef DynamicBuffer::operator[]( const std::string& key )
	{
		return { &(*pLayoutRoot)[key],bytes.data(),0u };
	}
	ConstElementRef DynamicBuffer::operator[]( const std::string& key ) const
	{
		auto dcb = const_cast<DynamicBuffer&>(*this)[key];
		return static_cast<ConstElementRef>(dcb);
	}
	const char* DynamicBuffer::GetData() const noexcept
	{
		return bytes.data();
	}
	size_t DynamicBuffer::GetSizeInBytes() const noexcept
	{
		return bytes.size();
	}
	const LayoutElement& DynamicBuffer::GetRootLayoutElement() const noexcept
	{
		return *pLayoutRoot;
	}
	void DynamicBuffer::CopyFrom( const DynamicBuffer& other )
	{
		assert( &GetRootLayoutElement() == &other.GetRootLayoutElement() );
		std::copy( other.bytes.begin(),other.bytes.end(),bytes.begin() );
	}
	std::shared_ptr<LayoutElement> DynamicBuffer::ShareLayoutRoot() const noexcept
	{
		return pLayoutRoot;
	}
}