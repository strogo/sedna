/*
 * File: xmlserializer.h
 * Copyright (C) 2010 The Institute for System Programming of the Russian Academy of Sciences (ISP RAS)
 */

#ifndef XMLSERIALIZER_H_
#define XMLSERIALIZER_H_

#include "common/base.h"
#include "common/sedna.h"

#include "tr/crmutils/xdm.h"
#include "tr/crmutils/serialization.h"
#include "tr/crmutils/str_matcher.h"

#include <map>
#include <stack>

struct dynamic_context;
struct ElementContext;
class StrMatcher;

enum pat_class
{
    pat_attribute = 1,
    pat_element = 2,
    pat_cdata = 4,
    pat_text = 8,
    pat_charmap = 64
};


class XDMSerializer : public Serializer {
  private:
    typedef std::map<std::string, xmlns_ptr> NSPrefixMap;
    typedef std::pair<xmlns_ptr, xmlns_ptr> NSSubsitutionPair;
    typedef std::stack< std::pair<xmlns_ptr, xmlns_ptr> > NSNamespaceStack;
    typedef std::map<xmlns_ptr, xmlns_ptr> NSSwizzlingMap;

    NSPrefixMap nsPrefixMap;
    NSNamespaceStack nsNamespaceStack;
    NSSwizzlingMap nsSwizzlingMap;
  protected:
    bool separatorNeeded;

    void printNode(const Node node);
    void printNode(IXDMNode * node);

    /*  declareNamespace appears in traversing the element subtree if element
      has or implies any namespace declaration. If namespace is unknown or swizzeled
      function returns true. */
    bool declareNamespace(xmlns_ptr ns);

    /* undeclareNamespaces undeclares "count" namespaces from stack */
    void undeclareNamespaces(int count);

    virtual void printAtomic(const tuple_cell &t) = 0;
    virtual void printDocument(const text_source_t docname, IXDMNode * content) = 0;
    virtual void printElement(IXDMNode * element) = 0;
    virtual void printNamespace(xmlns_ptr ns) = 0;
    virtual void printAttribute(IXDMNode * attribute) = 0;
    virtual void printText(t_item type, const text_source_t value) = 0;
  public:
    XDMSerializer();

    virtual void serialize(tuple &t);
};

class XMLSerializer : public XDMSerializer {
  private:
    bool indentNext;
    int indentLevel;
    int useCharmapFlag;
  protected:
    ElementContext * elementContext;
    StrMatcher stringFilter;

    virtual void printAtomic(const tuple_cell &t);
    virtual void printDocument(const text_source_t docname, IXDMNode * content);
    virtual void printElement(IXDMNode * element);
    virtual void printNamespace(xmlns_ptr ns);
    virtual void printAttribute(IXDMNode * attribute);
    virtual void printText(t_item type, const text_source_t value);
  public:
    XMLSerializer();
    ~XMLSerializer();

    virtual bool supports(enum se_output_method method) { return method == se_output_method_xml; };
    virtual void initialize();
};

/** SXML serializer outputs a tuple Scheme list form. This is still needed
 *  for a Sedna scheme driver that is used at least at our testing system.
 */

class SXMLSerializer : public XMLSerializer {
private:
    virtual void printText(t_item type, const text_source_t value);
    virtual void printAtomic(const tuple_cell &t);
    virtual void printDocument(const text_source_t docname, IXDMNode * content);
    virtual void printElement(IXDMNode * element);
    virtual void printAttribute(IXDMNode * attribute);
public:
    SXMLSerializer();
    ~SXMLSerializer();

    virtual bool supports(enum se_output_method method) { return method == se_output_method_sxml; };
};


#endif /* XMLSERIALIZER_H_ */