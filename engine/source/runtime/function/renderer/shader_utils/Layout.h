#pragma once
#include <optional>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <unordered_map>
#include "core/math/Vector.h"
#include "core/math/Matrix.h"

#define LEAF_ELEMENT_TYPES \
    X(Float) \
    X(Float2) \
    X(Float3) \
    X(Float4) \
    X(Mat3) \
    X(Mat4) \
    X(Bool) \
    X(Integer) \

namespace TinyEngine::Rendering
{
    enum class Type
    {
        #define X(type) type,
        LEAF_ELEMENT_TYPES
        #undef X
        Struct,
        Array,
        Empty
    };

    template<Type type>
    struct Map { static constexpr bool valid = false; };
    
    #define MAPTYPE(TYPE, SYSTYPE, CODE) \
    template<> struct Map<Type::TYPE> { \
        using SysType = SYSTYPE; \
        static constexpr size_t hlslSize = sizeof(SysType); \
        static constexpr const char* code = CODE; \
        static constexpr bool valid = true; \
    };
    MAPTYPE(Float, float, "F1");
    MAPTYPE(Float2, Vector2, "F2");
    MAPTYPE(Float3, Vector3, "F3");
    MAPTYPE(Float4, Vector4, "F4");
    MAPTYPE(Mat3, Matrix3x3, "M3");
    MAPTYPE(Mat4, Matrix4x4, "M4");
    MAPTYPE(Integer, int, "I1");
    template<> struct Map<Type::Bool> { 
        using SysType = bool;                           // type used in the CPU side
        static constexpr size_t hlslSize = 4;           // size in bytes in the GPU side
        static constexpr const char* code = "B1";       // code used for generating signature of layout
        static constexpr bool valid = true;             // metaprogramming flag to check validity of Map <param>
    };
    #undef MAPTYPE
    
    #define X(el) static_assert(Map<Type::el>::valid, "Missing map implementation for " #el);
    LEAF_ELEMENT_TYPES
    #undef X

    template<typename T>
    struct ReverseMap { static constexpr bool valid = false; };
    #define X(el) \
    template<> struct ReverseMap<Map<Type::el>::SysType> { \
        static constexpr Type type = Type::el; \
        static constexpr bool valid = true; \
    };
    LEAF_ELEMENT_TYPES
    #undef X

    #define X(el) static_assert(ReverseMap<Map<Type::el>::SysType>::valid, "Missing map implementation for " #el);
    LEAF_ELEMENT_TYPES
    #undef X

    class LayoutElement
    {
    private:
        struct ExtraDataBase
        {
            virtual ~ExtraDataBase() = default;
        };
        friend struct ExtraData;
        friend class RawLayout;
    public:
        std::string GetSignature() const noexcept;
        bool Exists() const noexcept;
        std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const noexcept;
        
        LayoutElement& operator[](const std::string& key) noexcept;
        const LayoutElement& operator[](const std::string& key) const noexcept;
        // T() only works for Arrays; gets the array type layout object
		// needed to further configure an array's type
        LayoutElement& T() ;
		const LayoutElement& T() const ;
        // offset based- functions only work after finalization!
		size_t GetOffsetBegin() const ;
		size_t GetOffsetEnd() const ;
		// get size in bytes derived from offsets
		size_t GetSizeInBytes() const ;

        // only works for Structs; add LayoutElement to struct
		LayoutElement& Add(Type addedType,std::string name );
		template<Type typeAdded>
		LayoutElement& Add(std::string key) {return Add(typeAdded,std::move(key));}
		// only works for Arrays; set the type and the # of elements
		LayoutElement& Set(Type addedType,size_t size );
		template<Type typeAdded>
		LayoutElement& Set(size_t size ){return Set( typeAdded,size );}
		// returns offset of leaf types for read/write purposes w/ typecheck in Debug
        template<typename T>
		size_t Offset() const
		{
			switch( type )
			{
			#define X(el) case Type::el: assert(typeid(Map<Type::el>::SysType) == typeid(T)); return *offset;
			LEAF_ELEMENT_TYPES
			#undef X
			default:
				assert( "Tried to resolve non-leaf element" && false );
				return 0u;
			}
		}
    private:
        // construct an empty layout element
		LayoutElement() noexcept = default;
		LayoutElement(Type typeIn);
        // sets all offsets for element and subelements, prepending padding when necessary
		// returns offset directly after this element
		size_t Finalize( size_t offsetIn );
		// implementations for GetSignature for aggregate types
		std::string GetSignatureForStruct() const;
		std::string GetSignatureForArray() const;
		// implementations for Finalize for aggregate types
		size_t FinalizeForStruct( size_t offsetIn );
		size_t FinalizeForArray( size_t offsetIn );
		// returns singleton instance of empty layout element
        static LayoutElement& GetEmptyElement() noexcept
		{
			static LayoutElement empty{};
			return empty;
		}
        // returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
        static size_t AdvanceToBoundary( size_t offset ) noexcept;
        // return true if a memory block crosses a boundary
		static bool CrossesBoundary( size_t offset,size_t size ) noexcept;
		// advance an offset to next boundary if block crosses a boundary
		static size_t AdvanceIfCrossesBoundary( size_t offset,size_t size ) noexcept;
		// check string for validity as a struct key
		static bool ValidateSymbolName( const std::string& name ) noexcept;
    private:

        std::optional<size_t> offset;
        Type type = Type::Empty;
        std::unique_ptr<ExtraDataBase> pExtraData;
    };

    class Layout
    {
    public:
        size_t GetSizeInBytes() const noexcept;
        std::string GetSignature() const noexcept;
    protected:
        Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
        std::shared_ptr<LayoutElement> pRoot;
    };

    class RawLayout : public Layout
    {
        friend class LayoutCodex;
    public:
        RawLayout() noexcept;
        // key into the root Struct
        LayoutElement& operator[](const std::string& key);
        // add a new element to the root Struct
        template<Type type>
        LayoutElement& Add(const std::string& key)
        {
            return pRoot->Add<type>(key);
        }
    private:
        void ClearRoot() noexcept;
        std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
    };

    /*
        CookedLayout is a Layout that has been finalized and is ready to be used
    */
    class CookedLayout : public Layout
    {
        friend class LayoutCodex;
        friend class DynamicBuffer;
    public:
        const LayoutElement& operator[](const std::string& key) const;
        
        std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
        
    private:
        CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;
        std::shared_ptr<LayoutElement> RelinquishRoot() const noexcept;
    };

    class ConstElementRef
    {
        friend class ElementRef;
        friend class DynamicBuffer;
    public:
        class Ptr
		{
			friend ConstElementRef;
		public:
			// conversion for getting read-only pointer to supported SysType
			template<typename T>
			operator const T*() const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*ref);
			}
		private:
			Ptr( const ConstElementRef* ref ) noexcept;
			const ConstElementRef* ref;
		};
    public:
        // check if the indexed element actually exists
        // this is possible because if you key into a Struct with a nonexistent key
        // it will still return an Empty LayoutElement that will enable this test
        // but will not enable any other kind of access
        bool Exists() const noexcept;
        // key into the current element as a struct
        ConstElementRef operator[]( const std::string& key ) const;
        // index into the current element as an array
        ConstElementRef operator[]( size_t index ) const;
        // emit a pointer proxy object
        Ptr operator&() const;
        // conversion for reading as a supported SysType
        template<typename T>
        operator const T&() const
        {
            static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in conversion");
            return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Offset<T>());
        }
    private:
        // refs should only be constructable by other refs or by the buffer
        ConstElementRef( const LayoutElement* pLayout,const char* pBytes,size_t offset ) noexcept;
        // this offset is the offset that is built up by indexing into arrays
        // accumulated for every array index in the path of access into the structure
        size_t offset;
        const LayoutElement* pLayout;
        const char* pBytes;
    };

    class ElementRef
    {
        friend class DynamicBuffer;
    public:
        class Ptr
		{
			friend ElementRef;
		public:
			// conversion to read/write pointer to supported SysType
			template<typename T>
			operator T*() const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*ref);
			}
		private:
			Ptr( ElementRef* ref ) noexcept;
			ElementRef* ref;
		};
    public:
        operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;
		ElementRef operator[]( const std::string& key ) const ;
		ElementRef operator[]( size_t index ) const ;
		// optionally set value if not an empty Ref
		template<typename S>
		bool SetIfExists( const S& val ) 
		{
			if( Exists() )
			{
				*this = val;
				return true;
			}
			return false;
		}
		Ptr operator&() const ;
		// conversion for reading/writing as a supported SysType
		template<typename T>
		operator T&() const 
		{
            // here always assert this error, have not find reason.
			// static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in assignment");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Offset<T>());
		}
		// assignment for writing to as a supported SysType
		template<typename T>
		T& operator=( const T& rhs ) const 
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid,"Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}
	private:
		// refs should only be constructable by other refs or by the buffer
		ElementRef( const LayoutElement* pLayout,char* pBytes,size_t offset ) noexcept;
		size_t offset;
		const LayoutElement* pLayout;
		char* pBytes;
    };
    
    class LayoutCodex
	{
	public:
		static CookedLayout Resolve( RawLayout&& layout );
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string,std::shared_ptr<LayoutElement>> map;
	};
}
