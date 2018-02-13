#pragma once

#include <utility>
#include <system_error>

namespace cct
{

template <typename T>
class error_and : std::pair<std::error_code, T>
{
public:
    template <typename...Args>
    explicit error_and(Args&&... args) : std::pair<std::error_code, T>{ std::forward<Args>(args)... } {}

    auto const& error_code() const & noexcept { return first; }

    auto& error_code() & noexcept { return first; }

    auto const& value() const& noexcept { return second; }

    auto& value() & noexcept { return second; }
    
    decltype(auto) value() && noexcept { return std::move(second); }

    auto* operator->() noexcept { return &second; }

    auto const* operator->() const noexcept { return &second; }
};

} // namespace cct
