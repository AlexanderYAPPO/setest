/*
    Copyright 2012 Ilya Taranov <ilya@taranoff.net>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "xmldiff.h"

#include <iostream>
#include <set>
#include <algorithm>

#include <stdexcept>
#include <exception>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <glibmm.h>
#include <assert.h>
#include <libxml++/libxml++.h>
#include <libxml/tree.h>

using namespace xmlpp;

XmlDiff::~XmlDiff() {}

XmlDiff::XmlDiff(std::istream& a, std::istream& b)
{
    parserA.parse_stream(a);

    if (!parserA) {
        throw std::runtime_error("Cannot parse document");
    }

    parserB.parse_stream(b);

    if (!parserB) {
        throw std::runtime_error("Cannot parse document");
    }
}

Element * descNode(Element * diff, Node * a)
{
    Element * nodeDsc = diff->add_child("node");
    nodeDsc->set_attribute("name", a->get_name());
    nodeDsc->set_attribute("line", boost::lexical_cast<Glib::ustring>(a->get_line()));

    if (!a->get_namespace_uri().empty()) {
        nodeDsc->set_attribute("uri", a->get_namespace_uri());
    }

    nodeDsc->add_child("path")->add_child_text(a->get_path());

    return nodeDsc;
};

/* TODO : implement less comparison over (Attribute *)
 *      not to copy Attribute ustrings  */

typedef std::set<std::string> AttributeSet;

void attributeListToSet(const Element::AttributeList &alist, AttributeSet & aout) {
    BOOST_FOREACH(Attribute *attr, alist) {
        aout.insert(std::string(attr->get_name()) + "=" + std::string(attr->get_value()));
    }
};

bool XmlDiff::compareNodes(Node * a, Node * b) {
    #ifdef SETEST_DEBUG
        std::cout << "TEST======!======TEST" << std::endl;
	std::cout << a->get_name() << std::endl;
	std::cout << b->get_name() << std::endl;
	std::cout << "END=======!=======END" << std::endl;
    #endif
    if (a->cobj()->type != b->cobj()->type
      || (a->get_name().compare(b->get_name()) != 0)
      || (a->get_namespace_uri().compare(b->get_namespace_uri()) != 0))
    {
        Element * p = result->add_child("different_elements");

        descNode(p, a);
        descNode(p, b);

        return false;
    }

    if (Attribute * asAttribute = dynamic_cast<Attribute *>(a)) {
        assert(false);
    }

    if (Attribute * asAttribute = dynamic_cast<Attribute *>(b)) {
        assert(false);
    }

    if (Element * asElement = dynamic_cast<Element *>(a)) {
        Element * bsElement = dynamic_cast<Element *>(b);

        assert(bsElement != NULL);

        AttributeSet asetA;
        AttributeSet asetB;
        AttributeSet difference;
        
        attributeListToSet(asElement->get_attributes(), asetA);
        attributeListToSet(asElement->get_attributes(), asetB);

        std::set_symmetric_difference(
            asetA.begin(), asetA.end(),
            asetB.begin(), asetB.end(),
              std::insert_iterator<AttributeSet>(difference, difference.begin()));

        if (!difference.empty()) {
            Element * p = result->add_child("different_attributes");

            BOOST_FOREACH(const Glib::ustring & attr, difference) {
                Element * attrE = p->add_child("attribute");
                attrE->add_child_text(attr);
/* TODO:
                Element * attrE = descNode(p, attr);

                attrE->set_attribute("type", "attribute");
                attrE->add_child("value")->add_child_text(attr->get_value());
*/                
            };
            
            descNode(p, b);
            descNode(p, a);
        };

        Element::NodeList childrenA = asElement->get_children();
        Element::NodeList childrenB = bsElement->get_children();
        
        Element::NodeList::iterator childAIt = childrenA.begin();
        Element::NodeList::iterator childBIt = childrenB.begin();

        while (childAIt != childrenA.end() && childBIt != childrenB.end()) {
            if (!compareNodes(*childAIt, *childBIt)) {
                return false;
            };

            childAIt++;
            childBIt++;
        };

        if ((childAIt != childrenA.end()) || (childBIt != childrenB.end())) {
            Element * p = result->add_child("additional_children");

            if (childAIt != childrenA.end()) {
                p->set_attribute("in", "original");
                descNode(p, a);
            };

            if (childBIt != childrenB.end()) {
                p->set_attribute("in", "new");
                descNode(p, b);
            };
            
            return false;
        };

        return true;
    };

    if (ContentNode * asCn = dynamic_cast<ContentNode *>(a)) {
        ContentNode * bsCn = dynamic_cast<ContentNode *>(b);
        
        assert(bsCn != NULL);

        if (asCn->get_content().compare(bsCn->get_content()) != 0) {
            Element * p = result->add_child("different_content");

            p->add_child("origin")->add_child_text(asCn->get_content());
            p->add_child("new")->add_child_text(bsCn->get_content());

            return false;
        };
        
        return true;
    };

    return false;
};

bool XmlDiff::diff()
{
    resultDoc = new Document();
    result = resultDoc->create_root_node("diff");

    return !compareNodes(
        parserA.get_document()->get_root_node(),
        parserB.get_document()->get_root_node());
}


