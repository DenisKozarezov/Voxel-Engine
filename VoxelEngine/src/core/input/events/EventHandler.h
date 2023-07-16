#pragma once
#include <core/PrimitiveTypes.h>
#include <functional>

namespace VoxelEngine::input
{
    class IEventHandler
    {
    public:
        virtual const string& getName() const = 0;
    };

    template <typename ...Args>
    class EventHandler : public IEventHandler
    {
    public:
        using Callback = std::function<bool(Args...)>;

        explicit EventHandler(const string& name, const Callback& cb) : _name(name), _cbFunc(cb) {}
        
        ~EventHandler() noexcept = default;

        virtual const string& getName() const override { return this->_name; }

        void invoke(Args... a) { this->_cbFunc(a...); }
    private:
        string _name;
        Callback const _cbFunc;
    };
}