#pragma once
#include <streams/BaseTokenOutputStream.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <string>
#include <deque>
#include <iostream>
#include <memory>

namespace tokenize
{

using OWrapper = rapidjson::OStreamWrapper;
using JSONWriter = rapidjson::Writer<OWrapper>;
using PrettyJSONWriter = rapidjson::PrettyWriter<OWrapper>;
class TokenJSONOutputStream : public BaseTokenOutputStream
{
private:
    std::shared_ptr<JSONWriter> writer;
    OWrapper ows;
    TokenPtr pending;

protected:
    bool next() override;

public:
    TokenJSONOutputStream(
        std::ostream & os_,
        BaseTokenInputStream & input_,
        bool pretty=false);
    bool write() override;
    void start() override;
    void finish() override;
    bool eos() const override;
};
}

