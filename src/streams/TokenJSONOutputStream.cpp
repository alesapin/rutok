#include <streams/TokenJSONOutputStream.h>

namespace tokenize
{

TokenJSONOutputStream::TokenJSONOutputStream(
    std::ostream & os_,
    BaseTokenInputStream & input_,
    bool pretty)
    : BaseTokenOutputStream(input_, 0)
    , ows(os_)
{
    if (pretty)
        writer = std::make_shared<PrettyJSONWriter>(ows);
    else
        writer = std::make_shared<JSONWriter>(ows);
}

void TokenJSONOutputStream::start()
{
    writer->StartArray();
}

void TokenJSONOutputStream::finish()
{
    writer->EndArray();
}


bool TokenJSONOutputStream::next()
{
    if (input.eof())
        return false;

    pending = input.read();
}

namespace
{

void writeString(std::shared_ptr<JSONWriter> writer, const std::string & value)
{
    writer->String(value.c_str(), value.length());
}
}

bool TokenJSONOutputStream::write()
{
    if (pending == nullptr)
        if (!next())
            return false;

    /// For last character
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
    pending = nullptr;
    return true;
}

bool TokenJSONOutputStream::eos() const
{
    return pending == nullptr && input.eof();
}
}
