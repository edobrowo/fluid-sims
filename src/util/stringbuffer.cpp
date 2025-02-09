#include "stringbuffer.hpp"

StringBuffer::StringBuffer()
    : mCapacity(INITIAL_CAPACITY),
      mNext(0),
      mBuffer(new char[INITIAL_CAPACITY]) {
}

StringBuffer::~StringBuffer() {
    delete[] mBuffer;
}

bool StringBuffer::isEmpty() {
    return mNext == 0;
}

Size StringBuffer::length() {
    return mNext;
}

const char* StringBuffer::str() {
    expand(mNext + 1);
    mBuffer[mNext] = '\0';
    return mBuffer;
}

std::string StringBuffer::toString() {
    const char* s = str();
    return std::string(s);
}

void StringBuffer::put(const char c) {
    mBuffer[mNext++] = c;
}

void StringBuffer::putSafe(const char c) {
    expand(mNext + 1);
    put(c);
}

void StringBuffer::append(const char* str, const Size length) {
    expand(mNext + length);
    std::memcpy(mBuffer + mNext, str, sizeof(char) * length);
    mNext += length;
}

void StringBuffer::append(const std::string& str) {
    expand(mNext + str.size());
    std::memcpy(mBuffer + mNext, str.c_str(), str.size());
    mNext += str.size();
}

void StringBuffer::expand(const Size size) {
    if (size > mCapacity) {
        mCapacity += (size / CAPACITY_INCREMENT + 1) * CAPACITY_INCREMENT;
        char* new_buffer = new char[mCapacity];
        std::memcpy(new_buffer, mBuffer, sizeof(char) * mNext);
        delete[] mBuffer;
        mBuffer = new_buffer;
    }
}
