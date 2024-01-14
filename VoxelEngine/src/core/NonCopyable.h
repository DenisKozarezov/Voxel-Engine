#pragma once

class NonCopyable
{
private:
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable(NonCopyable&&) = delete;
public:
    NonCopyable() = default;
    NonCopyable& operator=(NonCopyable const& rhs) = delete;
    NonCopyable& operator=(NonCopyable&& rhs) = delete;
};
