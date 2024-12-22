#pragma once

#include <iostream>
#include <string>

class ICard {
public:
    virtual ~ICard() = default;

    virtual short getValue() const = 0;
    virtual short getColor() const = 0;
    virtual void setColor(short _color) = 0;
    virtual bool isIllusion() const = 0;
    virtual bool isJustReturned() const = 0;
    virtual void setIllusion() = 0;
    virtual void resetIllusion() = 0;
    virtual void setJustReturned() = 0;
    virtual void resetJustReturned() = 0;

    virtual bool operator==(const ICard& other) const = 0;
    virtual bool operator<(const ICard& rhs) const = 0;
    virtual std::ostream& operator<<(std::ostream& os) const = 0;
};

