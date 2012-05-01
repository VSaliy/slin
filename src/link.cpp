#include "link.hpp"

slin::Link::Link()
{}

slin::Link::Link(std::string title, std::string link, std::string description)
    : Title{title}, Url{link}, Description{description}, m_id{-1}
{
}

int slin::Link::GetID() const
{
    return this->m_id;
}

void slin::Link::AddTag(std::string tag)
{
    this->Tags.insert(tag);
}

void slin::Link::AddNote(std::string text)
{
    this->Notes.insert(text);
}
