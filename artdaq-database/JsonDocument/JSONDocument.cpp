#include "artdaq-database/JsonDocument/common.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/basic/helpers.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:Document_C"

namespace regex
{
constexpr auto parse_value = "^\\{[^:]+:\\s+([\\s\\S]+)\\}$";
}


namespace artdaq {
namespace database {
namespace jsonutils {

using value_type_t = bsoncxx::type;

template <typename T>
constexpr std::uint8_t static_cast_as_uint8_t(T const& t)
{
    return static_cast<std::uint8_t>(t);
}


std::string JSONDocument::validate(std::string const& json)
{
    assert(!json.empty());

    TRACE_(10, "validate() json=<" << json << ">");

    auto document = bsoncxx::from_json(json);

    if (!document) {
        TRACE_(10, "validate()" << "Invalid JSON document.");

        throw cet::exception("JSONDocument") << "Invalid JSON document; json=" << json;
    }

    return bsoncxx::to_json(document->view());
}

std::string JSONDocument::value(JSONDocument const& document)
{
    auto ex = std::regex( {regex::parse_value});

    auto results = std::smatch();

    if (!std::regex_search(document._json_buffer, results, ex))
        throw cet::exception("JSONDocument")
                << ("Regex search failed; JSON buffer:" + document._json_buffer);

    if (results.size() != 2) {
        //we are interested in a second match
        TRACE_(12, "value()" << "JSON regex_search() result count=" << results.size());
        for (auto const & result : results) {
            TRACE_(12, "value()" << "JSON regex_search() result=" << result);
        }

        throw cet::exception("JSONDocument")
                << ("Regex search failed, regex_search().size()!=1; JSON buffer: " + document._json_buffer);
    }

    auto match = std::string(results[1]);

    match.erase(match.find_last_not_of(" \n\r\t") + 1);

    auto dequote = [](auto s) {

        if (s[0] == '"' && s[s.length() - 1] == '"')
            return s.substr(1, s.length() - 2);
        else
            return s;
    };

    match = dequote(match);

    TRACE_(12, "value()" << "JSON regex_search() result=" << match);

    return match;
}


std::string JSONDocument::value_at(JSONDocument const& document,std::size_t index) {
    auto values = bsoncxx::from_json(JSONDocument::value(document));

    auto arrayView = values->view();

    if (arrayView.empty()) {
        TRACE_(14, "value_at() Failed to call value_at(); arrayView is empty, document=<" << document._json_buffer << ">");
        throw cet::exception("JSONDocument") << "Failed to call value_at(); arrayView is empty, document=<" << document._json_buffer << ">";
    }

    if (std::distance(arrayView.cbegin() , arrayView.cend()) < int(index)) {
        TRACE_(14, "value_at() Failed to call value_at(); not enough elements, document=<" << document._json_buffer << ">");
        throw cet::exception("JSONDocument") << "Failed to call value_at(); not enough elements, document=<" << document._json_buffer << ">";
    }

    auto pos=arrayView.begin();
    std::advance(pos,index);

    return  bsoncxx::to_json(pos->get_value());
}

JSONDocument JSONDocument::loadFromFile(std::string const& fileName)
{
    assert(!fileName.empty());

    std::ifstream is;
    is.open(fileName.c_str());

    if (!is.good()) {
        is.close();
        TRACE_(11, "loadFromFile()" << "Failed opening a JSON file=" << fileName);

        throw cet::exception("JSONDocument") << "Failed opening a JSON file=" << fileName;
    }

    std::string json_buffer((std::istreambuf_iterator<char>(is)),
                            std::istreambuf_iterator<char>());

    is.close();

    return {json_buffer};
}

std::vector<std::string> JSONDocument::_split_path(std::string const& path) const
{
    auto tmp = std::string {path};

    TRACE_(1, "split_path() args path=<" << path << ">");

    std::replace(tmp.begin(), tmp.end(), '.', ' ');

    std::istringstream iss(tmp);

    std::vector<std::string> tokens {
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    if (!tokens.empty()) {
        std::stringstream ss;
        for (auto const & token : tokens)
            ss << "\"" << token << "\",";
        TRACE_(1, "split_path() loop token=<" << ss.str() << ">");
    } else {
        TRACE_(1, "split_path() token=<empty> ");
    }

    return  tokens;
}

//returns inserted child
JSONDocument JSONDocument::insertChild(JSONDocument const& newChild , path_t const& path)
{
    assert(!path.empty());
    assert(!newChild._json_buffer.empty());

    TRACE_(2, "insertChild() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(2, "insertChild() args  newChild=<" << newChild._json_buffer << ">");
    TRACE_(2, "insertChild() args  path=<" << path << ">");

    auto path_tokens = _split_path(path);

    if (path_tokens.empty())
        throw cet::exception("JSONDocument") << "Invalid path; path=" << path;

    std::reverse(path_tokens.begin(), path_tokens.end());

    auto newChildDocument = bsoncxx::from_json(newChild._json_buffer);
    auto newChildValue = newChildDocument->view().begin()->get_value();
    auto hasPayload = newChildDocument->view().begin()->key().to_string().compare("payload");

    if (!newChildDocument) {
        TRACE_(2, "insertChild()" << "Invalid newChild");

        throw cet::exception("JSONDocument") << "Invalid newChild; json_buffer=" << newChild._json_buffer;
    }

    auto initialDocument = bsoncxx::from_json(_json_buffer);

    if (!initialDocument) {
        TRACE_(2, "insertChild()" << " Invalid initialDocument");

        throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
    }

    using bsoncxx::builder::stream::document;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    auto wasUpdated = bool {false};

    std::function < document(view const&, std::size_t) > recurse =
    [&](view const & currentView, std::size_t currentDepth) {
        using namespace bsoncxx::builder::stream;

        auto resultDocument = document {};

        TRACE_(2, "insertChild() recurse() args currentDepth=" << currentDepth);
        TRACE_(2, "insertChild() recurse() args currentView=<" << bsoncxx::to_json(currentView) << ">");

        if (currentDepth == 0) {
            if (!currentView.empty()) {
                for (auto const & childElement : currentView) {
                    TRACE_(2, "insertChild() recurse() insert() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");
                    TRACE_(2, "insertChild() recurse() insert() childDocument=<" <<  bsoncxx::to_json(childElement.get_value()) << ">");

                    if (path_tokens.at(currentDepth) == childElement.key().to_string()) {
                        TRACE_(2, "insertChild() Insert failed path =<" << path << ">, call replaceChild() instead.");
                        throw cet::exception("JSONDocument") << "Insert failed path =<" << path << ">, call replaceChild() instead.";
                    } else {
                        resultDocument <<  childElement.key().to_string() << childElement.get_value();
                    }
                }
            }

            resultDocument << path_tokens.at(currentDepth) << newChildValue;

            TRACE_(2, "insertChild() recurse() insert() newChildDocument=<" <<    bsoncxx::to_json(bsoncxx::types::b_document {newChildDocument->view()}) << ">");

            wasUpdated = true;

            TRACE_(2, "insertChild() recurse() resultDocument=<" << bsoncxx::to_json(resultDocument) << ">");
            TRACE_(2, "insertChild() recurse() Insert succeeded.");

            return resultDocument;
        }

        auto matchedPath = bool {false};

        for (auto const & childElement : currentView) {
            TRACE_(2, "insertChild() recurse() copy() childElement=<" << bsoncxx::to_json(childElement) << ">");

            auto childType = static_cast_as_uint8_t(childElement.type());

            TRACE_(2, "insertChild() recurse() copy() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");

            switch (childType) {
            default: {
                resultDocument <<  childElement.key().to_string() << childElement.get_value() ;
                break;
            }

            case static_cast_as_uint8_t(value_type_t::k_document): {
                auto childView = childElement.get_value().get_document();

                TRACE_(2, "insertChild() recurse() copy() childView=<" << bsoncxx::to_json(childView) << ">");

                if (path_tokens.at(currentDepth) == childElement.key().to_string()) {
                    resultDocument <<  childElement.key().to_string() << open_document << concatenate {recurse(childView, currentDepth - 1)} << close_document;
                    matchedPath = true;
                } else {
                    resultDocument <<  childElement.key().to_string() << open_document << concatenate {childView} << close_document;
                    TRACE_(2, "insertChild() recurse() copy() resultDocument=<" << bsoncxx::to_json(resultDocument) << ">");
                }
                break;
            }
            }
        }

        if (!matchedPath) {
            TRACE_(2, "insertChild() recurse() Error: Invalid JSON insert path =<" << path << ">");

            throw cet::exception("JSONDocument") << "Invalid JSON insert path =<" << path << ">";
        }

        return resultDocument;
    };

    auto updatedDocument = recurse(initialDocument->view(), path_tokens.size() - 1);

    if (!wasUpdated) {
        TRACE_(2, "insertChild() Error: Insert failed path =<" << path << ">, call replaceChild() instead.");

        throw cet::exception("JSONDocument") << "Inset failed path =<" << path << ">, call replaceChild() instead.";
    }

    _json_buffer = bsoncxx::to_json(updatedDocument);

    TRACE_(2, "insertChild() end  _json_buffer=<" << _json_buffer << ">");

    return (hasPayload==0) ? bsoncxx::to_json(newChildValue) :  newChild;
}

//returns old child
JSONDocument JSONDocument::replaceChild(JSONDocument const& newChild, path_t const& path)
{
    assert(!path.empty());
    assert(!newChild._json_buffer.empty());

    TRACE_(4, "replaceChild() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(4, "replaceChild() args  newChild=<" << newChild._json_buffer << ">");
    TRACE_(4, "replaceChild() args  path=<" << path << ">");

    auto path_tokens = _split_path(path);

    if (path_tokens.empty())
        throw cet::exception("JSONDocument") << "Invalid path; path=" << path;

    std::reverse(path_tokens.begin(), path_tokens.end());

    auto newChildDocument = bsoncxx::from_json(newChild._json_buffer);

    if (!newChildDocument) {
        TRACE_(4, "replaceChild()" << "Invalid newChild");

        throw cet::exception("JSONDocument") << "Invalid newChild; json_buffer=" << newChild._json_buffer;
    }

    auto newChildView = newChildDocument->view();

    auto newChildKey=newChildView.begin()->key().to_string();

    if(newChildKey.compare("payload")==0) {
        newChildView = newChildView.begin()->get_value().get_document().value;
        newChildKey =newChildView.begin()->key().to_string();
    }

    auto initialDocument = bsoncxx::from_json(_json_buffer);

    if (!initialDocument) {
        TRACE_(4, "replaceChild()" << " Invalid initialDocument");

        throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
    }

    using bsoncxx::builder::stream::document;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    auto wasUpdated = bool {false};

    auto replaced_json_buffer =  std::string {};

    std::function < document(view const&, std::size_t) > recurse =
    [&](view const & currentView, std::size_t currentDepth) {
        using namespace bsoncxx::builder::stream;

        auto resultDocument = document {};

        TRACE_(4, "replaceChild() recurse() args currentDepth=" << currentDepth);
        TRACE_(4, "replaceChild() recurse() args currentView=<" << bsoncxx::to_json(currentView) << ">");

        if (currentDepth == 0) {
            for (auto const & childElement : currentView) {
                TRACE_(4, "replaceChild() recurse() replace() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");
                TRACE_(4, "replaceChild() recurse() replace() childDocument=<" <<  bsoncxx::to_json(childElement.get_value()) << ">");

                if (path_tokens.at(currentDepth) == childElement.key().to_string()) {

                    if(childElement.key().to_string() != newChildKey) {
                         resultDocument << path_tokens.at(currentDepth) << newChildDocument->view().begin()->get_value();
                         replaced_json_buffer = bsoncxx::to_json(childElement.get_value());
                    }
                    else {
                        resultDocument  << path_tokens.at(currentDepth) << newChildView.begin()->get_value();
                        replaced_json_buffer = std::string("{").append(bsoncxx::to_json(childElement)).append(" }");
                    }

                    TRACE_(4, "replaceChild() recurse() replace() newChildDocument=<" <<    bsoncxx::to_json(bsoncxx::types::b_document {newChildView}) << ">");

                    wasUpdated = true;
                } else {
                    resultDocument <<  childElement.key().to_string() << childElement.get_value();
                }
            }

            TRACE_(4, "replaceChild() recurse() resultDocument=<" << bsoncxx::to_json(resultDocument) << ">");
            TRACE_(4, "replaceChild() recurse() Replace succeeded.");

            return resultDocument;
        }

        auto matchedPath = bool {false};

        for (auto const & childElement : currentView) {
            TRACE_(4, "replaceChild() recurse() copy() childElement=<" << bsoncxx::to_json(childElement) << ">");

            auto childType = static_cast_as_uint8_t(childElement.type());

            TRACE_(4, "replaceChild() recurse() copy() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");

            switch (childType) {
            default: {
                resultDocument <<  childElement.key().to_string() << childElement.get_value() ;
                break;
            }

            case static_cast_as_uint8_t(value_type_t::k_document): {
                auto childView = childElement.get_value().get_document();

                TRACE_(4, "replaceChild() recurse() copy() childView=<" << bsoncxx::to_json(childView) << ">");

                if (path_tokens.at(currentDepth) == childElement.key().to_string()) {
                    resultDocument <<  childElement.key().to_string() << open_document << concatenate {recurse(childView, currentDepth - 1)} << close_document;
                    matchedPath = true;
                } else {
                    resultDocument <<  childElement.key().to_string() << open_document << concatenate {childView} << close_document;
                    TRACE_(4, "replaceChild() recurse() copy() resultDocument=<" << bsoncxx::to_json(resultDocument) << ">");
                }
                break;
            }
            }
        }

        if (!matchedPath) {
            TRACE_(4, "replaceChild() recurse() Error: Invalid JSON insert path =<" << path << ">");

            throw cet::exception("JSONDocument") << "Invalid JSON insert path =<" << path << ">";
        }

        return resultDocument;
    };

    auto updatedDocument = recurse(initialDocument->view(), path_tokens.size() - 1);

    if (!wasUpdated) {
        TRACE_(4, "replaceChild() Error: Replace failed path =<" << path << ">, call insertChild() instead.");

        throw cet::exception("JSONDocument") << "Replace failed path =<" << path << ">, call insertChild() instead.";
    }

    _json_buffer = bsoncxx::to_json(updatedDocument);

    TRACE_(4, "replaceChild() end  _json_buffer=<" << _json_buffer << ">");

    return {replaced_json_buffer};
}

//returns old child
JSONDocument JSONDocument::deleteChild(path_t const& path)
{
    assert(!path.empty());

    TRACE_(3, "deleteChild() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(3, "deleteChild() args  path=<" << path << ">");

    auto path_tokens = _split_path(path);

    if (path_tokens.empty())
        throw cet::exception("JSONDocument") << "Invalid paht; path=" << path;

    std::reverse(path_tokens.begin(), path_tokens.end());

    auto initialDocument = bsoncxx::from_json(_json_buffer);

    if (!initialDocument) {
        TRACE_(3, "deleteChild()" << " Invalid initialDocument");

        throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
    }

    using bsoncxx::builder::stream::document;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    auto wasUpdated = bool {false};

    auto deleted_json_buffer =  std::string {};

    std::function < document(view const&, std::size_t) > recurse =
    [&](view const & currentView, std::size_t currentDepth) {
        using namespace bsoncxx::builder::stream;

        auto resultDocument = document {};

        TRACE_(3, "deleteChild() recurse() args currentDepth=" << currentDepth);
        TRACE_(3, "deleteChild() recurse() args currentView=<" << bsoncxx::to_json(currentView) << ">");

        if (currentDepth == 0) {
            for (auto const & childElement : currentView) {
                TRACE_(3, "deleteChild() recurse() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");
                TRACE_(3, "deleteChild() recurse() childElement=<" << bsoncxx::to_json(childElement) << ">");

                if (path_tokens.at(currentDepth) == childElement.key().to_string()) {
		    deleted_json_buffer = bsoncxx::to_json(childElement.get_value());

		    if(path == "_id") {
		        std::replace(deleted_json_buffer.begin(),deleted_json_buffer.end(),'$','_');
		    }
		    
                    TRACE_(3, "deleteChild() recurse() Delete succeeded.");
                    wasUpdated = true;
                    continue;
                }
   
		
		resultDocument << childElement.key().to_string() << childElement.get_value();
            }

            return resultDocument;
        }

        auto matchedPath = bool {false};

        for (auto const & childElement : currentView) {
            TRACE_(3, "deleteChild() recurse() childElement=<" << bsoncxx::to_json(childElement) << ">");

            auto childView = childElement.get_value().get_document();

            TRACE_(3, "deleteChild() recurse() childView=<" << bsoncxx::to_json(childView) << ">");
            TRACE_(3, "deleteChild() recurse() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");

            if (path_tokens.at(currentDepth) == childElement.key().to_string()) {
                resultDocument <<  childElement.key().to_string() << open_document << concatenate {recurse(childView, currentDepth - 1)} << close_document;
                matchedPath = true;
            } else {
                resultDocument <<  childElement.key().to_string() << open_document << concatenate {childView} << close_document;
                TRACE_(3, "deleteChild() recurse() resultDocument=<" << bsoncxx::to_json(resultDocument) << ">");
            }
        }

        if (!matchedPath) {
            TRACE_(3, "deleteChild() recurse() Error: Invalid JSON delete path =<" << path << ">");

            throw cet::exception("JSONDocument") << "Invalid JSON delete path =<" << path << ">";
        }

        return resultDocument;
    };

    auto updatedDocument = recurse(initialDocument->view(), path_tokens.size() - 1);

    if (!wasUpdated) {
        TRACE_(3, "deleteChild() recurse() Error: Delete failed path =<" << path << ">, call findChild() instead.");

        throw cet::exception("JSONDocument") << "Delete failed path =<" << path << ">, call findChild() instead.";
    }

    _json_buffer = bsoncxx::to_json(updatedDocument);

    TRACE_(3, "deleteChild() end  _json_buffer=<" << _json_buffer << ">");

    return {deleted_json_buffer};
}

//returns found child
JSONDocument JSONDocument::findChild(path_t const& path) const
{
    assert(!path.empty());

    TRACE_(5, "findChild() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(5, "findChild() args  path=<" << path << ">");

    auto path_tokens = _split_path(path);

    if (path_tokens.empty())
        throw cet::exception("JSONDocument") << "Invalid paht; path=" << path;

    std::reverse(path_tokens.begin(), path_tokens.end());

    auto initialDocument = bsoncxx::from_json(_json_buffer);

    if (!initialDocument) {
        TRACE_(5, "findChild()" << " Invalid initialDocument");

        throw cet::exception("JSONDocument") << " Invalid initialDocument; json_buffer=" << _json_buffer;
    }

    using bsoncxx::builder::stream::document;
    using bsoncxx::document::view;
    using bsoncxx::document::value;

    auto foundChild = bool {false};

    std::function < document(view const&, std::size_t) > recurse =
    [&](view const & currentView, std::size_t currentDepth) {
        using namespace bsoncxx::builder::stream;

        auto resultDocument = document {};

        TRACE_(5, "findChild() recurse() args currentDepth=" << currentDepth);
        TRACE_(5, "findChild() recurse() args currentView=<" << bsoncxx::to_json(currentView) << ">");

        if (currentDepth == 0) {
            for (auto const & childElement : currentView) {
                TRACE_(5, "findChild() recurse() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");
                TRACE_(5, "findChild() recurse() childElement=<" << bsoncxx::to_json(childElement) << ">");

                if (path_tokens.at(currentDepth) == childElement.key().to_string()) {
                    TRACE_(5, "findChild() recurse() Search succeeded.");
                    resultDocument <<  childElement.key().to_string() << childElement.get_value();
                    foundChild = true;
                    break;
                }
            }

            return resultDocument;
        }

        auto matchedPath = bool {false};

        for (auto const & childElement : currentView) {
            TRACE_(5, "findChild() recurse() childElement=<" << bsoncxx::to_json(childElement) << ">");

            if (path_tokens.at(currentDepth) != childElement.key().to_string())
                continue;

            assert(childElement.type() == value_type_t::k_document);

            if (childElement.type() != value_type_t::k_document) {
                TRACE_(5, "findChild() recurse() Error: Coding error, JSON search path =<" << path << ">");
                throw cet::exception("JSONDocument") << "Coding error, JSON search path =<" << path << ">";
            }

            auto childView = childElement.get_value().get_document();

            TRACE_(5, "findChild() recurse() childView=<" << bsoncxx::to_json(childView) << ">");
            TRACE_(5, "findChild() recurse() path,key=<" << path_tokens.at(currentDepth) << "," << childElement.key().to_string() << ">");

            resultDocument = recurse(childView, currentDepth - 1) ;

            matchedPath = true;
            break;
        }

        if (!matchedPath) {
            TRACE_(5, "findChild() recurse() Error: Invalid JSON search path =<" << path << ">");

            throw notfound_exception("JSONDocument") << "Invalid JSON search path =<" << path << ">";
        }

        return resultDocument;
    };

    auto childDocument = recurse(initialDocument->view(), path_tokens.size() - 1);

    if (!foundChild) {
        TRACE_(5, "findChild() recurse() Error: Search failed path =<" << path << ">, call findChild() instead.");

        throw notfound_exception("JSONDocument") << "Search failed path =<" << path << ">, call findChild() instead.";
    }

    auto json_buffer = bsoncxx::to_json(childDocument);

    TRACE_(5, "findChild() end  json_buffer=<" << json_buffer << ">");
    TRACE_(5, "findChild() end  _json_buffer=<" << _json_buffer << ">");

    return {json_buffer};
}


bool JSONDocument::matches(JSONDocument const& other, bool matchSize) const
{
    TRACE_(10, "operator==() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(10, "operator==() args  other.__json_buffer=<" << other._json_buffer << ">");

    if (other._json_buffer == _json_buffer)
        return true;

    auto thisDocument = bsoncxx::from_json(_json_buffer);
    auto otherDocument = bsoncxx::from_json(other._json_buffer);

    using bsoncxx::builder::stream::document;
    using bsoncxx::document::view;
    using bsoncxx::document::value;
    using bsoncxx::document::element;

    std::function < bool (view , view , path_t) > areEqual =
    [&areEqual, &matchSize](view thisView, view otherView, path_t path) {
        TRACE_(10, "operator==() areEqual this=<"  << bsoncxx::to_json(thisView) << ">");
        TRACE_(10, "operator==() areEqual other=<" << bsoncxx::to_json(otherView) << ">");
        TRACE_(10, "operator==() areEqual path=<"  << path << ">");

        //both are empty
        if (thisView.empty() && otherView.empty()) {
            return true;
        }

        //different sizes
        if (matchSize) {
            if (thisView.empty() || otherView.empty()) {
                return false;
            } else if (std::distance(thisView.cbegin() , thisView.cend())
            != std::distance(otherView.cbegin(), otherView.cend())) {
                return false;
            }
        }

        for (auto thisChild : thisView) {
            auto key_name = thisChild.key().to_string();

            auto otherViewChildIter = otherView.find(key_name);

            if (otherViewChildIter == otherView.end()) {
                //skip missing elements
                if (matchSize) {
                    TRACE_(10, "operator==() Other is missing an element, path=<" << path << "." << key_name << ">");
                    return false;
                } else {
                    continue;
                }
            }

            auto otherChild = *otherViewChildIter;

            if (thisChild.type() != otherChild.type()) {
                TRACE_(10, "operator==() Elements have different types, path=<" << path << "." << key_name << ">");
                return false;
            }

            auto tmpThis = thisChild.get_value();
            auto tmpOther = otherChild.get_value();

            switch (static_cast_as_uint8_t(thisChild.type())) {
            default: {
                TRACE_(10, "operator==() checking value, path=<" << path << "." << key_name << ">");

                if (tmpThis != tmpOther) {
                    TRACE_(10, "operator==() Elements have different values, path=<" << path << "." << key_name << ">");
                    return false;
                }

                break;
            }

            case static_cast_as_uint8_t(value_type_t::k_array): {
                TRACE_(10, "operator==() checking k_array, path=<" << path << "." << key_name << ">");

                if(tmpThis.get_array().value.empty() && tmpThis.get_array().value.empty()) {
                    break;
                } else if(tmpThis.get_array().value.empty() || tmpThis.get_array().value.empty()) {
                    return false;
                }

                if (! areEqual(tmpThis.get_array().value, tmpOther.get_array().value, path.append(".").append(key_name)))
                    return false;

                break;
            }

            case static_cast_as_uint8_t(value_type_t::k_document): {
                TRACE_(10, "operator==() checking k_document, path=<" << path << "." << key_name << ">");

                if (! areEqual(tmpThis.get_document().value, tmpOther.get_document().value, path.append(".").append(key_name)))
                    return false;

                break;
            }
            }

        }

        //no discrepancies were found
        return true;
    };

    auto result = areEqual(thisDocument->view(), otherDocument->view(), "");

    TRACE_(10, "operator==() end, JSON buffers are " << (result ? "equal." : "not equal."));

    return result;
}

//returns added child
JSONDocument JSONDocument::appendChild(JSONDocument const& child , path_t const& path)
{
    assert(!path.empty());

    TRACE_(13, "appendChild() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(13, "appendChild() args  child=<" << child._json_buffer << ">");
    TRACE_(13, "appendChild() args  path=<" << path << ">");

    using bsoncxx::builder::stream::array;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::array_context;

    auto searchResults = findChild(path);

    auto searchResultsDocument = bsoncxx::from_json(searchResults._json_buffer);
    auto searchResultsView = searchResultsDocument->view();

    if (std::distance(searchResultsView.begin(), searchResultsView.end()) != 1) {
        TRACE_(13, "appendChild() Too many elements in the results view, path=<" << path << ">");
        throw cet::exception("JSONDocument") << "Too many elements in the results view, path=<" << path << ">";
    }

    auto firstResultIt = searchResultsView.begin();

    if (firstResultIt->get_value().type() != value_type_t::k_array) {
        TRACE_(13, "appendChild() Not an array element, path=<" << path << ">");
        throw cet::exception("JSONDocument") << "Failed to appendChild(); target path is not an array element, path=<" << path << ">";
    }

    auto newChildDocument = bsoncxx::from_json(child._json_buffer);
    auto newChildValue = newChildDocument->view().begin()->get_value();

    auto newArray = array {};

    newArray << open_array <<
    [&](array_context<> tmp_array_context) {
        if (!firstResultIt->get_value().get_array().value.empty()) {
            for (auto const & childElement : firstResultIt->get_value().get_array().value)
                tmp_array_context << childElement.get_value();
        }
        TRACE_(13, "appendChild() appending _json_buffer =<" << bsoncxx::to_json(newChildValue) << ">");
        tmp_array_context << newChildValue;
    } << close_array;

    auto updatedDocument = JSONDocument( {bsoncxx::to_json(newArray.view())});

    TRACE_(13, "appendChild() updatedDocument._json_buffer =<" << updatedDocument._json_buffer << ">");

    replaceChild(updatedDocument, path);


    return {bsoncxx::to_json(newChildValue)};
}

//returns removed child
JSONDocument JSONDocument::removeChild(JSONDocument const& child , path_t const& path)
{
    assert(!path.empty());

    TRACE_(13, "removeChild() begin _json_buffer=<" << _json_buffer << ">");
    TRACE_(13, "removeChild() args  child=<" << child._json_buffer << ">");
    TRACE_(13, "removeChild() args  path=<" << path << ">");

    using bsoncxx::builder::stream::array;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::array_context;

    auto matchFilter = JSONDocument {JSONDocument::value(child)};

    auto searchResults = findChild(path);
    auto searchResultsDocument = bsoncxx::from_json(searchResults._json_buffer);
    auto searchResultsView = searchResultsDocument->view();

    if (std::distance(searchResultsView.begin(), searchResultsView.end()) != 1) {
        TRACE_(13, "removeChild() Too many elements in the results view, path=<" << path << ">");
        throw cet::exception("JSONDocument") << "Too many elements in the results view, path=<" << path << ">";
    }

    auto firstResultIt = searchResultsView.begin();

    if (firstResultIt->get_value().type() != value_type_t::k_array) {
        TRACE_(13, "removeChild() Not an array element, path=<" << path << ">");
        throw cet::exception("JSONDocument") << "Failed to removeChild(); target path is not an array element, path=<" << path << ">";
    }

    auto deleted_json_buffer = std::string();
    auto newArray = array {};



    newArray << open_array <<
    [&](array_context<> tmp_array_context) {

        if (firstResultIt->get_value().get_array().value.empty()) {
            TRACE_(13, "removeChild() Failed to removeChild(); element array is empty, path=<" << path << ">");
            throw cet::exception("JSONDocument") << "Failed to removeChild(); element array is empty, path=<" << path << ">";
        }

        auto addComma = bool {false};
        std::stringstream  ss;
        ss << "{\"0\": [";

        for (auto const & childElement : firstResultIt->get_value().get_array().value) {
            auto  json_buffer = bsoncxx::to_json(childElement.get_value());
            if (!matchFilter.matches( {json_buffer}, false)) {
                tmp_array_context << childElement.get_value();
                TRACE_(13, "removeChild() copied element, json_buffer =<" << bsoncxx::to_json(childElement.get_value()) << ">");
            }
            else {
                ss << (addComma ? ',' : ' ') << std::move(json_buffer);
                addComma = true;
                TRACE_(13, "removeChild() filtered element, json_buffer =<" << deleted_json_buffer << ">");
            }
        }

        ss << "]}";

        deleted_json_buffer = ss.str();
    } << close_array;

    auto updatedDocument = JSONDocument( {bsoncxx::to_json(newArray.view())});

    TRACE_(13, "removeChild() updatedDocument._json_buffer =<" << updatedDocument._json_buffer << ">");

    replaceChild(updatedDocument, path);

    return {deleted_json_buffer};
}

bool JSONDocument::isReadonly() const {
    return false;
}

void  trace_enable_JSONDocument()
{
    TRACE_CNTL("name",    TRACE_NAME);
    TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
    TRACE_CNTL("modeM", 1LL);
    TRACE_CNTL("modeS", 1LL);

    TRACE_(0, "artdaq::database::JSONDocument::" << "trace_enable");
}

std::ostream& operator<<(std::ostream& os, JSONDocument const& document)
{
    os << document.to_string();

    return os;
}

} //namespace jsonutils
} //namespace database
} //namespace artdaq
