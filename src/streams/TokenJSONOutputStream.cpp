#include <streams/TokenJSONOutputStream.h>

namespace tokenize
{

TokenJSONOutputStream::TokenJSONOutputStream(
    BaseCharOutputStream & output_,
    bool pretty_)
    : BaseTokenOutputStream(0)
    , ows(oss)
    , output(output_)
    , pretty(pretty_)
{
    if (pretty)
    {
        writer_ptr = std::make_shared<PrettyJSONWriter>(ows);
        std::get<PrettyWriterPtr>(writer_ptr)->SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
    }
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
}


bool TokenJSONOutputStream::next(TokenPtr token)
{
    return true;
}

void TokenJSONOutputStream::flush()
{
    if (pretty)
        std::get<PrettyWriterPtr>(writer_ptr)->Flush();
    else
        std::get<WriterPtr>(writer_ptr)->Flush();

    if (oss)
    {
        output.write(oss.str());
        oss.clear();
        oss.str("");
    }

    output.flush();
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

    writer->Key("semantic_tags");
    writer->StartArray();
    if (pending->getSemanticTag() != ESemanticTag::UNKNOWN)
        for (auto tag : toTagSet(pending->getSemanticTag()))
            writeString(writer, toString(tag));
    writer->EndArray();

    writer->EndObject();
}
}

void TokenJSONOutputStream::write(TokenPtr token)
{

    if (pretty)
    {
        writeOneToken(std::get<PrettyWriterPtr>(writer_ptr), token);
        std::get<PrettyWriterPtr>(writer_ptr)->Flush();
    }
    else
    {
        writeOneToken(std::get<WriterPtr>(writer_ptr), token);
        std::get<WriterPtr>(writer_ptr)->Flush();
    }
    output.write(oss.str());
    oss.str("");
    oss.clear();
}

}
