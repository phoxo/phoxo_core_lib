#pragma once

namespace WIC
{
    class MetadataIterator
    {
    public:
        virtual ~MetadataIterator() {}

        /// Calling this function in a constructor is prohibited because of callbacks during enum
        void EnumAllMetadata(IWICBitmapFrameDecode* frame_decode)
        {
            std::deque<IWICMetadataReaderPtr>   waiting_enum;
            GetFrameMetaReaders(frame_decode, waiting_enum);

            while (waiting_enum.size())
            {
                auto   reader = waiting_enum.front();
                waiting_enum.pop_front();
                EnumSingleReader(reader, waiting_enum);
            }
        }

    protected:
        /// meta_format - GUID_MetadataFormatIfd, GUID_MetadataFormatExif ...
        virtual void OnBeforeEnumReader(IWICMetadataReader* reader, REFGUID meta_format) {}

        virtual void OnEnumMetadataItem(REFGUID meta_format, const CComPROPVARIANT& item_id, const CComPROPVARIANT& val) {}

    private:
        void EnumSingleReader(IWICMetadataReader* reader, auto& waiting_enum)
        {
            UINT   item_count = GetMetadataItemCount(reader);
            if (!item_count)
                return;

            GUID   meta_format = GetMetadataFormat(reader);
            OnBeforeEnumReader(reader, meta_format);

            for (UINT i = 0; i < item_count; i++)
            {
                CComPROPVARIANT   id;
                CComPROPVARIANT   val;
                reader->GetValueByIndex(i, NULL, &id, &val);
                OnEnumMetadataItem(meta_format, id, val);

                if (val.vt == VT_UNKNOWN)
                {
                    IWICMetadataReaderPtr   child = val.punkVal; assert(child);
                    waiting_enum.push_back(child);
                }
            }
        }

        static void GetFrameMetaReaders(IWICBitmapFrameDecode* frame_decode, auto& out_list)
        {
            IWICMetadataBlockReaderPtr   block = frame_decode;
            UINT   count = GetFrameMetaReaderCount(block);
            for (UINT i = 0; i < count; i++)
            {
                IWICMetadataReaderPtr   t;
                block->GetReaderByIndex(i, &t);
                out_list.push_back(t);
            }
        }

        static UINT GetFrameMetaReaderCount(IWICMetadataBlockReader* frame)
        {
            UINT   t = 0;
            if (frame) { frame->GetCount(&t); }
            return t;
        }

        static UINT GetMetadataItemCount(IWICMetadataReader* reader)
        {
            UINT   t = 0;
            if (reader) { reader->GetCount(&t); }
            return t;
        }

        static GUID GetMetadataFormat(IWICMetadataReader* reader)
        {
            GUID   t = {};
            if (reader) { reader->GetMetadataFormat(&t); }
            return t;
        }
    };
}
