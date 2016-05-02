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


void EntityDescriptor::addEnumeration(const std::string &title, const std::vector<std::string> &entries) {
    std::string enumerate = "<p><b>" + title + "</b><ol>";
    for (std::string s : entries) {
        enumerate = enumerate + "<li>" + s + "</li>";
    }
    enumerate = enumerate + "</ol></p>";
    body.push_back(enumerate);
}


void EntityDescriptor::addItemize(const std::string &title, const std::vector<std::string> &entries) {
    std::string itemize = "<p><b>" + title + "</b><ul>";
    for (std::string s : entries) {
        itemize = itemize + "<li>" + s + "</li>";
    }
    itemize = itemize + "</ul></p>";
    body.push_back(itemize);
}


void EntityDescriptor::addFooter(const std::string &id, const std::string &created, const std::string &updated) {
    footer.push_back("<hr>");
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


void EntityDescriptor::addItem(const std::string &item) {
    body.push_back("<b> - " + item + "</b><br>");
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


std::string EntityDescriptor::value_to_str(const nix::Value &v, const nix::DataType &dtype) {
    std::string val;
    if(nix::data_type_is_numeric(dtype)) {
        switch (dtype) {
            case (nix::DataType::Double): {
                double d = v.get<double>();
                val = nix::util::numToStr(d);
                break;
            }
            case (nix::DataType::Int64): {
                int64_t i = v.get<int64_t>();
                val = nix::util::numToStr(i);
                break;
            }
            case (nix::DataType::Bool): {
                bool b = v.get<bool>();
                val = (b ? "True" : "False");
            }
            case (nix::DataType::UInt32): {
                uint32_t i = v.get<uint32_t>();
                val = nix::util::numToStr(i);
            }
            default:
                val = "";
        }
    } else if (dtype == nix::DataType::String) {
        v.get(val);
    }
    return val;
}


std::string EntityDescriptor::describe(const nix::DataArray &da) {
    EntityDescriptor desc(da.name(), da.type(), (da.definition() ? *da.definition() : "none"), da.id(),
                          nix::util::timeToStr(da.createdAt()), nix::util::timeToStr(da.updatedAt()));
    std::vector<std::string> dims;
    for (nix::Dimension d : da.dimensions()) {
        dims.push_back(nix::util::dimTypeToStr(d.dimensionType()));
    }
    desc.addEnumeration("Dimensions", dims);
    std::string s = " [";
    for (size_t i = 0; i < da.dataExtent().size(); i++) {
        s = s + nix::util::numToStr(da.dataExtent()[i]);
        if (i < da.dataExtent().size() - 1)
            s = s + ", ";
    }
    s = s + "]";
    desc.addInfo("Shape", s);
    desc.addInfo("Metadata", (da.metadata() ? da.metadata().name() : "none"));
    std::vector<std::string> sources;
    for (nix::Source src : da.sources()) {
        sources.push_back(src.name());
    }
    desc.addItemize("Sources", sources);
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Block &b){
    EntityDescriptor desc(b.name(), b.type(), (b.definition() ? *b.definition() : "none"), b.id(),
                          nix::util::timeToStr(b.createdAt()), nix::util::timeToStr(b.updatedAt()));
    desc.addInfo("Metadata", (b.metadata() ? b.metadata().name() : "none"));
    desc.addInfo("DataArrays", nix::util::numToStr(b.dataArrayCount()));
    desc.addInfo("Groups", nix::util::numToStr(b.groupCount()));
    desc.addInfo("Sources", nix::util::numToStr(b.sourceCount()));
    desc.addInfo("Tags", nix::util::numToStr(b.tagCount()));
    desc.addInfo("MultiTags", nix::util::numToStr(b.multiTagCount()));
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Tag &t) {
    EntityDescriptor desc(t.name(), t.type(), (t.definition() ? *t.definition() : "none"), t.id(),
                          nix::util::timeToStr(t.createdAt()), nix::util::timeToStr(t.updatedAt()));
    desc.addInfo("Metadata", (t.metadata() ? t.metadata().name() : "none"));
    std::vector<std::string> refs;
    for (nix::DataArray da : t.references()) {
        refs.push_back(da.name() + "[" + da.type() + "]");
    }
    std::vector<std::string> feats;
    for (nix::Feature f : t.features()) {
        feats.push_back(f.data().name() + "[" + f.data().type() + "]");
    }
    std::vector<std::string> sources;
    for (nix::Source src : t.sources()) {
        sources.push_back(src.name());
    }
    desc.addItemize("References", refs);
    desc.addItemize("Features", feats);
    desc.addItemize("Sources", sources);
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::MultiTag &t) {
    EntityDescriptor desc(t.name(), t.type(), (t.definition() ? *t.definition() : "none"), t.id(),
                          nix::util::timeToStr(t.createdAt()), nix::util::timeToStr(t.updatedAt()));
    desc.addInfo("Metadata", (t.metadata() ? t.metadata().name() : "none"));
    std::vector<std::string> refs;
    for (nix::DataArray da : t.references()) {
        refs.push_back(da.name() + "[" + da.type() + "]");
    }
    std::vector<std::string> feats;
    for (nix::Feature f : t.features()) {
        feats.push_back(f.data().name() + "[" + f.data().type() + "]");
    }
    std::vector<std::string> sources;
    for (nix::Source src : t.sources()) {
        sources.push_back(src.name());
    }
    desc.addItemize("References", refs);
    desc.addItemize("Features", feats);
    desc.addItemize("Sources", sources);
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Group &g) {
    EntityDescriptor desc(g.name(), g.type(), (g.definition() ? *g.definition() : "none"), g.id(),
                          nix::util::timeToStr(g.createdAt()), nix::util::timeToStr(g.updatedAt()));
    desc.addInfo("Metadata", (g.metadata() ? g.metadata().name() : "none"));
    desc.addInfo("DataArrays", nix::util::numToStr(g.dataArrayCount()));
    desc.addInfo("Tags", nix::util::numToStr(g.tagCount()));
    desc.addInfo("MultiTags", nix::util::numToStr(g.multiTagCount()));
    desc.addInfo("Sources", nix::util::numToStr(g.sourceCount()));
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Feature &f) {
    return EntityDescriptor::describe(f.data());
}


std::string EntityDescriptor::describe(const nix::Dimension &d) {
    EntityDescriptor desc;
    desc.name("Dimension descriptor");
    desc.addInfo("Index", nix::util::numToStr(d.index()));
    desc.addInfo("Type", nix::util::dimTypeToStr(d.dimensionType()));
    if (d.dimensionType() == nix::DimensionType::Sample) {
        nix::SampledDimension sam = d.asSampledDimension();
        desc.addInfo("Label", (sam.label() ? *sam.label() : ""));
        desc.addInfo("Sampling interval", nix::util::numToStr(d.asSampledDimension().samplingInterval()));
        desc.addInfo("Offset", (sam.offset() ? nix::util::numToStr(*sam.offset()) : "0.0"));
        desc.addInfo("Unit", (sam.unit() ? *sam.unit() : ""));
    } else if (d.dimensionType() == nix::DimensionType::Range) {
        nix::RangeDimension rdim = d.asRangeDimension();
        std::vector<double> ticks = rdim.ticks();
        desc.addInfo("Label", (rdim.label() ? *rdim.label() : ""));
        desc.addInfo("Ticks", nix::util::numToStr(ticks.size()) + "("+ nix::util::numToStr(ticks[0]) +
                     " ... " + nix::util::numToStr(ticks.back()) + ")");
        desc.addInfo("Unit", (rdim.unit() ? *rdim.unit() : ""));
    }
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Source &s) {
    EntityDescriptor desc(s.name(), s.type(), (s.definition() ? *s.definition() : "none"), s.id(),
                          nix::util::timeToStr(s.createdAt()), nix::util::timeToStr(s.updatedAt()));
    desc.addInfo("Metadata", (s.metadata() ? s.metadata().name() : "none"));
    std::vector<std::string> children;
    for (nix::Source src : s.sources()) {
        children.push_back(src.name());
    }
    desc.addItemize("Children", children);
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Section &s) {
    EntityDescriptor desc(s.name(), s.type(), (s.definition() ? *s.definition() : "none"), s.id(),
                          nix::util::timeToStr(s.createdAt()), nix::util::timeToStr(s.updatedAt()));
    std::vector<std::string> props;
    for (nix::Property p : s.properties()) {
        std::string val;
        if (p.valueCount() > 0) {
            nix::Value v  = p.values()[0];
            val = value_to_str(v, p.dataType());
            props.push_back(p.name() + ": " + (val.size() < 20 ? val : val.substr(20)));
        }
    }
    desc.addItemize("Properties", props);
    return desc.toHtml();
}


std::string EntityDescriptor::describe(const nix::Property &p) {
    EntityDescriptor desc(p.name(), "", (p.definition() ? *p.definition() : "none"), p.id(),
                          nix::util::timeToStr(p.createdAt()), nix::util::timeToStr(p.updatedAt()));
    desc.addInfo("Unit", (p.unit() ? *p.unit() : ""));
    std::vector<std::string> values;
    for (nix::Value v : p.values()) {
        values.push_back(value_to_str(v, p.dataType()));
    }
    desc.addItemize("Values", values);
    return desc.toHtml();
}
