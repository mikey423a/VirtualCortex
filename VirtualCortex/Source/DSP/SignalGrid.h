#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace vc {
// Portable, allocation-free routing *model*. used by the live and offline engines.
enum class BlockType : std::uint8_t { Empty, Gate, Amp, Cabinet, EQ, Delay, Reverb, Tremolo, Compressor, Overdrive, Distortion, Chorus, Flanger, Phaser, Wah, BitCrusher, Vibrato, RingModulator, Gain, LowHighCut, ToneEQ, RoomReverb, GridStereoDelay=30 };
struct BlockSlot { BlockType type = BlockType::Empty; bool enabled = true; };
class SignalGrid {
public:
 static constexpr std::size_t rows = 4, columns = 8;
 using Row = std::array<BlockSlot, columns>;
 using Grid = std::array<Row, rows>;
 bool set(std::size_t row, std::size_t column, BlockSlot slot) noexcept {
  if (!valid(row, column) || !validType(slot.type)) return false;
  slots_[row][column] = slot; return true;
 }
 bool swap(std::size_t rowA, std::size_t colA, std::size_t rowB, std::size_t colB) noexcept {
  if (!valid(rowA,colA) || !valid(rowB,colB)) return false;
  const auto temp = slots_[rowA][colA]; slots_[rowA][colA] = slots_[rowB][colB]; slots_[rowB][colB] = temp; return true;
 }
 bool bypass(std::size_t row, std::size_t column, bool bypassed) noexcept {
  if (!valid(row,column)) return false;
  slots_[row][column].enabled = !bypassed; return true;
 }
 const BlockSlot* get(std::size_t row, std::size_t column) const noexcept {
  return valid(row,column) ? &slots_[row][column] : nullptr;
 }
 const Grid& snapshot() const noexcept { return slots_; }
 static constexpr bool valid(std::size_t row, std::size_t col) noexcept { return row < rows && col < columns; }
 static constexpr bool validType(BlockType type) noexcept { return static_cast<unsigned>(type) <= static_cast<unsigned>(BlockType::RoomReverb) || type==BlockType::GridStereoDelay; }
private:
 Grid slots_{};
};
}
