//
// Created by Mike on 12/11/2019.
//

#pragma once

#include <cstdint>
#include <type_traits>
#include <iostream>
#include <array>

namespace gr {

constexpr bool is_prime(uint32_t x) {
    if (x == 0 || x == 1) { return false; }
    if (x == 2) { return true; }
    if (x % 2 == 0) { return false; }
    for (auto i = 3u; i * i <= x; i += 2) {
        if (x % i == 0) { return false; }
    }
    return true;
}

namespace impl {

template<uint32_t ...numbers>
using U32Sequence = std::integer_sequence<uint32_t, numbers...>;

template<typename AllNumbers, typename PrimeNumbers, uint32_t limit>
struct MakePrimeNumberSequenceImpl {};

template<uint32_t ...prime_numbers, uint32_t limit>
struct MakePrimeNumberSequenceImpl<U32Sequence<>, U32Sequence<prime_numbers...>, limit> {
    using Type = U32Sequence<prime_numbers...>;
};

template<uint32_t next_number, bool should_append, uint32_t ...prime_numbers>
struct SequenceAppendConditionalImpl {
    using Type = U32Sequence<prime_numbers...>;
};

template<uint32_t next_number, uint32_t ...prime_numbers>
struct SequenceAppendConditionalImpl<next_number, true, prime_numbers...> {
    using Type = U32Sequence<prime_numbers..., next_number>;
};

template<uint32_t curr, uint32_t ...other_numbers, uint32_t ...prime_numbers, uint32_t limit>
struct MakePrimeNumberSequenceImpl<U32Sequence<curr, other_numbers...>, U32Sequence<prime_numbers...>, limit> {

    using Type = typename MakePrimeNumberSequenceImpl<
        U32Sequence<other_numbers...>,
        typename SequenceAppendConditionalImpl<curr, curr < limit && is_prime(curr), prime_numbers...>::Type,
        limit>::Type;
};

// Divide the calculation into parts in order to surpass MSVC's limitation on template recursion depth
constexpr auto number_sequence_size_per_iteration = 128u;

template<typename Numbers, uint32_t scale, uint32_t offset>
struct MultiplyAndAddImpl {};

template<uint32_t ...numbers, uint32_t scale, uint32_t offset>
struct MultiplyAndAddImpl<U32Sequence<numbers...>, scale, offset> {
    using Type = U32Sequence<(numbers * scale + offset)...>;
};

template<uint32_t iter>
using NumberSequencePartImpl = typename MultiplyAndAddImpl<
    std::make_integer_sequence<uint32_t, number_sequence_size_per_iteration>,
    2,
    number_sequence_size_per_iteration * 2 * iter + 1>::Type;

template<
    uint32_t limit,
    uint32_t total_iter = (limit + number_sequence_size_per_iteration - 1) / number_sequence_size_per_iteration,
    uint32_t iter = 0,
    typename Sequence = U32Sequence<2u>>
struct PrimeNumberSequenceIterImpl {
    using Type = typename PrimeNumberSequenceIterImpl<
        limit,
        total_iter,
        iter + 1,
        typename MakePrimeNumberSequenceImpl<NumberSequencePartImpl<iter>, Sequence, limit>::Type>::Type;
};

template<uint32_t limit, uint32_t iter, typename Sequence>
struct PrimeNumberSequenceIterImpl<limit, iter, iter, Sequence> {
    using Type = Sequence;
};

template<typename Numbers>
struct MakeArrayFromSequenceImpl {};

template<uint32_t ...numbers>
struct MakeArrayFromSequenceImpl<U32Sequence<numbers...>> {
    static constexpr auto make() noexcept {
        return std::array<uint32_t, sizeof...(numbers)>{numbers...};
    }
};


template<size_t size>
static constexpr auto array_prefix_sum_impl(std::array<uint32_t, size> array, size_t index) noexcept {
    auto sum = 0u;
    for (auto i = 0ul; i <= index; i++) {
        sum += array[i];
    }
    return sum;
}

template<typename Indices>
struct MakePrefixSumArrayImpl {};

template<size_t ...indices>
struct MakePrefixSumArrayImpl<std::index_sequence<indices...>> {
    template<size_t size>
    static constexpr auto make(std::array<uint32_t, size> array) noexcept {
        return std::array<uint32_t, size>{array_prefix_sum_impl(array, indices)...};
    }
};

}

template<typename T>
constexpr auto make_array_from_sequence() noexcept {
    return impl::MakeArrayFromSequenceImpl<T>::make();
}

template<size_t size>
constexpr auto make_prefix_sum_array(std::array<uint32_t, size> array) noexcept {
    return impl::MakePrefixSumArrayImpl<std::make_index_sequence<size>>::make(array);
}

template<uint32_t limit>
using PrimeNumberSequence = typename impl::PrimeNumberSequenceIterImpl<limit>::Type;

using PrimeSequence = PrimeNumberSequence<7920>;

constexpr auto prime_number_array = make_array_from_sequence<PrimeSequence>();
constexpr auto prime_number_prefix_sum_array = make_prefix_sum_array(prime_number_array);

}

int main() {

    std::cout << gr::prime_number_array.size() << std::endl;

    std::cout << "====== NUMBERS ======" << std::endl;
    for (auto n : gr::prime_number_array) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    std::cout << "======= SUMS ========" << std::endl;
    for (auto s : gr::prime_number_prefix_sum_array) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

}
