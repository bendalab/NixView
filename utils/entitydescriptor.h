#ifndef ENTITYDESCRIPTOR_H
#define ENTITYDESCRIPTOR_H

#include <string>
#include <vector>
#include <nix.hpp>
#include <QVariant>

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
    void addItem(const std::string &item);
    void addInfo(const std::string &name, const std::string &value);
    void addEnumeration(const std::string &title, const std::vector<std::string> &entries);
    void addItemize(const std::string &title, const std::vector<std::string> &entries);

    void clear();

    std::string toHtml();
    static std::string describe(const QVariant &var);
    static std::string describe(const nix::DataArray &da);
    static std::string describe(const nix::Block &b);
    static std::string describe(const nix::Tag &t);
    static std::string describe(const nix::MultiTag &t);
    static std::string describe(const nix::Group &g);
    static std::string describe(const nix::Feature &f);
    static std::string describe(const nix::Dimension &g);
    static std::string describe(const nix::Source &s);
    static std::string describe(const nix::Section &s);
    static std::string describe(const nix::Property &p);
    static std::string value_to_str(const nix::Value &v, const nix::DataType &dtype);

private:
    std::string entity_name;
    std::vector<std::string> body;
    std::vector<std::string> footer;
};

#endif // ENTITYDESCRIPTOR_H
