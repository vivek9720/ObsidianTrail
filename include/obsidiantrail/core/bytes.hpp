#ifndef OBSIDIANTRAIL_CORE_BYTES_HPP
#define OBSIDIANTRAIL_CORE_BYTES_HPP

#include "obsidiantrail/core/result.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace obsidiantrail::core {

class ByteView {
public:
    ByteView() = default;
    ByteView(const std::uint8_t* data, std::size_t size) : data_(data), size_(size) {}
    explicit ByteView(const std::vector<std::uint8_t>& data) : data_(data.data()), size_(data.size()) {}
    const std::uint8_t* data() const { return data_; }
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    Result<std::uint8_t> at(std::size_t offset) const;
    Result<ByteView> slice(std::size_t offset, std::size_t length) const;
    std::vector<std::uint8_t> to_vector() const;
    std::string ascii_lossy() const;
private:
    const std::uint8_t* data_ = nullptr;
    std::size_t size_ = 0;
};

class Reader {
public:
    explicit Reader(ByteView view) : view_(view) {}
    std::size_t offset() const { return offset_; }
    std::size_t remaining() const;
    Result<std::uint8_t> u8();
    Result<std::uint16_t> be16();
    Result<std::uint32_t> be32();
    Result<std::uint64_t> be64();
    Result<std::uint16_t> le16();
    Result<std::uint32_t> le32();
    Result<std::uint64_t> varint();
    Result<ByteView> bytes(std::size_t count);
private:
    ByteView view_;
    std::size_t offset_ = 0;
};

std::uint32_t crc32(ByteView view);
std::uint64_t rolling_checksum(ByteView view);

} // namespace obsidiantrail::core

#endif
