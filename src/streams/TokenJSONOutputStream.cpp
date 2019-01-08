#include <streams/TokenJSONOutputStream.h>

namespace tokenize
{

TokenJSONOutputStream::TokenJSONOutputStream(
    std::ostream & os_,
    BaseTokenInputStream & input_,
    bool pretty_)
    : BaseTokenOutputStream(input_, 0)
    , ows(os_)
    , pretty(pretty_)
{
    if (pretty)
        writer_ptr = std::make_shared<PrettyJSONWriter>(ows);
    else
        writer_ptr = std::make_shared<JSONWriter>(ows);
}

void TokenJSONOutputStream::start()
{
    if (pretty)
        std::get<PrettyWriterPtr>(writer_ptr)->StartArray();
    else
        std::get<WriterPtr>(writer_ptr)->StartArray();
}

void TokenJSONOutputStream::finish()
{
    if (pretty)
        std::get<PrettyWriterPtr>(writer_ptr)->EndArray();
    else
        std::get<WriterPtr>(writer_ptr)->EndArray();
    flush();
}


bool TokenJSONOutputStream::next()
{
    if (input.eof())
        return false;

    pending = input.read();
    return true;
}

void TokenJSONOutputStream::flush()
{
    if (pretty)
        std::get<PrettyWriterPtr>(writer_ptr)->Flush();
    else
        std::get<WriterPtr>(writer_ptr)->Flush();
}

namespace
{
void writeString(std::shared_ptr<JSONWriter> writer, const std::string & value)
{
    writer->String(value.c_str(), static_cast<uint32_t>(value.length()));
}

template <typename T>
void writeOneToken(T writer, TokenPtr pending)
{
    writer->StartObject();
    writer->Key("text");
    writeString(writer, pending->getData());
    writer->Key("token_type");
    writeString(writer, toString(pending->getTokenType()));
    writer->Key("graphem_tags");
    writer->StartArray();
    if (pending->getGraphemTag() != EGraphemTag::UNKNOWN)
        for (auto tag : toTagSet(pending->getGraphemTag()))
            writeString(writer, toString(tag));

    writer->EndArray();
    writer->EndObject();
}
}

bool TokenJSONOutputStream::write()
{
    if (pending == nullptr)
        if (!next())
            return false;

    if (pretty)
        writeOneToken(std::get<PrettyWriterPtr>(writer_ptr), pending);
    else
        writeOneToken(std::get<WriterPtr>(writer_ptr), pending);

    pending = nullptr;
    return true;
}

bool TokenJSONOutputStream::eos() const
{
    return pending == nullptr && input.eof();
}
}
