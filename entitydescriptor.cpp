#include "entitydescriptor.h"

EntityDescriptor::EntityDescriptor()
{

}


EntityDescriptor::EntityDescriptor(const std::string &name, const std::string &type,
                                   const std::string &definition, const std::string &id,
                                   const std::string &created_at, const std::string &updated_at) :
    entity_name(name) {

    addInfo("type", type);
    addInfo("definition", definition);
    addSeparator();
    addFooter(id, created_at, updated_at);
}


void EntityDescriptor::name(const std::string &name) {
    this->entity_name = name;
}


std::string EntityDescriptor::name() const {
    return this->entity_name;
}


void EntityDescriptor::addInfo(const std::string &name, const std::string &value) {
    body.push_back("<p><b>" + name + ": </b>" + value + "</p>");
}


void EntityDescriptor::addFooter(const std::string &id, const std::string &created, const std::string &updated) {
    footer.push_back("<small><p><b>id: </b>" + id + "</p></small>");
    footer.push_back("<small><p><b>created at: </b>" + created + "</p></small>");
    footer.push_back("<small><p><b>updated at: </b>" + updated + "</p></small>");
}


void EntityDescriptor::addSeparator() {
    body.push_back("<hr>");
}


void EntityDescriptor::addSection(const std::string &name) {
    body.push_back("<h3>" + name + "</h3>");
}


void EntityDescriptor::clear() {
    this->footer.clear();
    this->body.clear();
    name("");
}


std::string EntityDescriptor::toHtml() {
    std::string html = "<html>";
    html = html + "<h2>" + this->entity_name + "</h2>";
    for (size_t i = 0; i < body.size(); i++) {
        html = html + body[i];
    }
    for (size_t i = 0; i < footer.size(); i++) {
        html = html + footer[i];
    }
    html = html + "</html>";
    return html;
}
