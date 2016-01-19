#ifndef ENTITYDESCRIPTOR_H
#define ENTITYDESCRIPTOR_H

#include <string>
#include <vector>

class EntityDescriptor
{
public:
    EntityDescriptor();
    EntityDescriptor(const std::string &name, const std::string &type, const std::string &definition,
                     const std::string &id, const std::string &created_at, const std::string &updated_at);

    void name(const std::string &name);
    std::string name() const;
    void addFooter(const std::string &id, const std::string &created, const std::string &updated);
    void addSeparator();
    void addSection(const std::string &name);
    void addInfo(const std::string &name, const std::string &value);

    void clear();

    std::string toHtml();

private:
    std::string entity_name;
    std::vector<std::string> body;
    std::vector<std::string> footer;
};

#endif // ENTITYDESCRIPTOR_H
