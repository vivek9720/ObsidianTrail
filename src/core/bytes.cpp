#include "obsidiantrail/core/bytes.hpp"
#include <algorithm>

namespace obsidiantrail::core {

Result<std::uint8_t> ByteView::at(std::size_t offset) const {
    if (offset >= size_) return Result<std::uint8_t>::err("byte.offset", "offset outside byte view");
    return Result<std::uint8_t>::ok(data_[offset]);
}

Result<ByteView> ByteView::slice(std::size_t offset, std::size_t length) const {
    if (offset > size_ || length > size_ - offset) return Result<ByteView>::err("byte.slice", "slice outside byte view");
    return Result<ByteView>::ok(ByteView(data_ + offset, length));
}

std::vector<std::uint8_t> ByteView::to_vector() const {
    return std::vector<std::uint8_t>(data_, data_ + size_);
}

std::string ByteView::ascii_lossy() const {
    std::string out;
    out.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        unsigned char ch = data_[i];
        out.push_back(ch >= 32 && ch < 127 ? static_cast<char>(ch) : ' ');
    }
    return out;
}

std::size_t Reader::remaining() const {
    return offset_ <= view_.size() ? view_.size() - offset_ : 0;
}

Result<std::uint8_t> Reader::u8() {
    auto r = view_.at(offset_);
    if (r) ++offset_;
    return r;
}

Result<std::uint16_t> Reader::be16() {
    auto b = bytes(2);
    if (!b) return Result<std::uint16_t>::err(b.code(), b.message());
    return Result<std::uint16_t>::ok(static_cast<std::uint16_t>((b.value().data()[0] << 8) | b.value().data()[1]));
}

Result<std::uint32_t> Reader::be32() {
    auto b = bytes(4);
    if (!b) return Result<std::uint32_t>::err(b.code(), b.message());
    const auto* p = b.value().data();
    return Result<std::uint32_t>::ok((std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[2]) << 8) | p[3]);
}

Result<std::uint64_t> Reader::be64() {
    auto hi = be32();
    auto lo = be32();
    if (!hi) return Result<std::uint64_t>::err(hi.code(), hi.message());
    if (!lo) return Result<std::uint64_t>::err(lo.code(), lo.message());
    return Result<std::uint64_t>::ok((std::uint64_t(hi.value()) << 32) | lo.value());
}

Result<std::uint16_t> Reader::le16() {
    auto b = bytes(2);
    if (!b) return Result<std::uint16_t>::err(b.code(), b.message());
    return Result<std::uint16_t>::ok(static_cast<std::uint16_t>(b.value().data()[0] | (b.value().data()[1] << 8)));
}

Result<std::uint32_t> Reader::le32() {
    auto b = bytes(4);
    if (!b) return Result<std::uint32_t>::err(b.code(), b.message());
    const auto* p = b.value().data();
    return Result<std::uint32_t>::ok(std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) | (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24));
}

Result<std::uint64_t> Reader::varint() {
    std::uint64_t value = 0;
    unsigned shift = 0;
    for (unsigned i = 0; i < 10; ++i) {
        auto byte = u8();
        if (!byte) return Result<std::uint64_t>::err(byte.code(), byte.message());
        value |= std::uint64_t(byte.value() & 0x7f) << shift;
        if ((byte.value() & 0x80) == 0) return Result<std::uint64_t>::ok(value);
        shift += 7;
    }
    return Result<std::uint64_t>::err("varint.long", "varint exceeds ten bytes");
}

Result<ByteView> Reader::bytes(std::size_t count) {
    auto r = view_.slice(offset_, count);
    if (r) offset_ += count;
    return r;
}

std::uint32_t crc32(ByteView view) {
    std::uint32_t crc = 0xffffffffu;
    for (std::size_t i = 0; i < view.size(); ++i) {
        crc ^= view.data()[i];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
        }
    }
    return ~crc;
}

std::uint64_t rolling_checksum(ByteView view) {
    std::uint64_t a = 1, b = 0;
    for (std::size_t i = 0; i < view.size(); ++i) {
        a = (a + view.data()[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 32) | a;
}

} // namespace obsidiantrail::core
