#include <fmt/core.h>
#include <iostream>
#include <string>
#include <valarray>
#include <variant>
#include "common/Logger.hpp"

struct iProperties
{
    virtual double getMin() const = 0;
    virtual double getMax() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool isInRange() const = 0;
    virtual double getVal() const = 0;
    virtual void setMin(double) = 0;
    virtual void setMax(double) = 0;
    virtual void setName(std::string) = 0;
    virtual void setDescription(std::string) = 0;
};

template <typename T>
struct iPropertiesCRTP : iProperties
{
    double getMin() const override { return T::min; }
    double getMax() const override { return T::max; }
    double getVal() const override { return dynamic_cast<const T*>(this)->value; }
    std::string getName() const override { return T::name; }
    std::string getDescription() const override { return T::description; }
    bool isInRange() const override { return getMin() <= getVal() and getVal() <= getMax(); }
    void setMin(double) override {}
    void setMax(double) override {}
    void setName(std::string) override {}
    void setDescription(std::string) override {}
};

struct DynamicProperties : iProperties
{
    DynamicProperties(double min, double max, std::string description, std::string name)
        : min{min}, max{max}, description{std::move(description)}, name{std::move(name)}
    {
    }

    double getMin() const override { return min; }
    double getMax() const override { return max; }
    std::string getName() const override { return name; }
    std::string getDescription() const override { return description; }
    bool isInRange() const override { return getMin() <= getVal() and getVal() <= getMax(); }

    void setMin(double newMin) override { min = newMin; }
    void setMax(double newMax) override { max = newMax; }
    void setName(std::string newName) override { name = std::move(newName); }
    void setDescription(std::string newDescription) override { description = std::move(newDescription); }

private:
    double min;
    double max;
    std::string description;
    std::string name;
};

struct tint : iPropertiesCRTP<tint>
{
    int value;
    tint(int val) : value{val} {}

    static constexpr double min{0.0};
    static constexpr double max{100.0};
    inline static const std::string description{"no nie wiem co jest grane"};
    inline static const std::string name{"int - basic int type with range 0 - 100"};
};

struct dint : DynamicProperties
{
    int value;
    dint(int val) : DynamicProperties{0, 100, "desc - missing", "name - missing"}, value{val} {}
};
//****************
struct noise
{
    float alpha;
    int beta;

    /* constexpr_in_some_future_standard */
    constexpr float operator()(float input) const { return std::fmod(std::pow(input, alpha), float(beta)) / beta; }
};

//***********************
int main(int, char**)
{
    tint ti{10};
    log_inf("test logger!!!");
    fmt::print("is in range = {} ({})\n", ti.isInRange(), ti.getName());
    ti.value = {20};

    // tint ti2{20};

    noise n{0.3f, 12};
    float f = n(23.3f);
    fmt::print("f = {}\n", f);
    fmt::print("is in range = {} ({})\n", ti.isInRange(), ti.getDescription());
    ti.value = 101;
    fmt::print("is in range = {} ({})\n", ti.isInRange(), ti.getName());
    return 0;
}
