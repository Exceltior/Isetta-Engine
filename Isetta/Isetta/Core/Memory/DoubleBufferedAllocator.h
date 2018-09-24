/*
 * Copyright (c) 2018 Isetta
 */
#pragma once
#include "Core/IsettaAlias.h"
#include "Core/Memory/StackAllocator.h"

namespace Isetta {
// TODO(YIDI): This class hasn't been tested
/**
 * \brief A double buffered allocator is a wrapper on top of the stack
 * allocator. It holds two stack allocators (A, B) and marks one of them as an
 * active stack allocator each frame. At the end of each frame, it swaps A and B
 * and clears the newly activated one. For example, if A is the active allocator
 * of frame n, all memory allocation requests will be handled by A during frame
 * n. At the end of frame n, B becomes the active allocator and is cleared. So
 * during frame n + 1, memory allocation will be done by B, and those pointers
 * to memory in A are still valid. At the end of n + 1, A is again activated and
 * cleared.
 */
class DoubleBufferedAllocator {
 public:
  ~DoubleBufferedAllocator() = default;

 private:
  /**
   * \brief Does nothing. This exists only to be called by the constructor of
   * MemoryManager
   */
  DoubleBufferedAllocator() = default;

  /**
   * \brief Create a new double buffered allocator with specified size
   * \param size Size in bytes
   */
  explicit DoubleBufferedAllocator(Size size);

  /**
   * \brief Grab unaligned raw memory from the double buffered allocator. The
   * memory will be automatically cleared at the end of next frame
   *
   * \param size Size in bytes
   * \return Raw memory pointer, use with caution!
   */
  void* AllocUnAligned(Size size);

  /**
   * \brief Grab properly aligned raw memory from the double buffered allocator.
   * The memory will be automatically cleared at the end of next frame
   *
   * \param size Size in bytes
   * \param alignment Alignment requirement for this memory. Must be power of 2
   * and less than 256 \return Raw memory pointer, use with caution!
   */
  void* Alloc(Size size, U8 alignment = 16);

  /**
   * \brief Swap the active stack allocator
   */
  void SwapBuffer();

  /**
   * \brief Clears the currently active stack allocator
   */
  void ClearCurrentBuffer();

  /**
   * \brief Erase both stack allocators
   */
  void Erase();

  /**
   * \brief Creates a new object on the active stack allocator, which will
   * become invalid at the end of next frame. You need to manually call
   * destructor on the object if needed. \tparam T \return
   */
  template <typename T>
  T* New();

  StackAllocator stacks[2];
  U8 curStackIndex;
  friend class MemoryManager;
};

template <typename T>
T* DoubleBufferedAllocator::New() {
  void* mem = Alloc(sizeof(T));
  return new (mem) T();
}

}  // namespace Isetta
