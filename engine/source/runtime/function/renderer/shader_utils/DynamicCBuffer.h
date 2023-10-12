#pragma once
#include "Layout.h"

namespace TinyEngine::Rendering
{
    
    class DynamicBuffer
    {
    public:
        DynamicBuffer(RawLayout&& layout) ;
        DynamicBuffer(CookedLayout&& layout) ;
        DynamicBuffer(const CookedLayout& layout) ;
        DynamicBuffer(const DynamicBuffer& other) noexcept ;
        // have to be careful with this one...
		// the buffer that has once been pilfered must not be used :x
        DynamicBuffer(DynamicBuffer&& other) noexcept ;
		// how you begin indexing into buffer (root is always Struct)
		ElementRef operator[]( const std::string& key ) ;
        // if Buffer is const, you only get to index into the buffer with a read-only proxy
		ConstElementRef operator[]( const std::string& key ) const ;
        // get the raw bytes
		const char* GetData() const noexcept;
		// size of the raw byte buffer
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetRootLayoutElement() const noexcept;
		// copy bytes from another buffer (layouts must match)
		void CopyFrom( const DynamicBuffer& );
		// return another sptr to the layout root
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;
    private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
    };
}
