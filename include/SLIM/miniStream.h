#ifndef MINI_STREAM_H
#define MINI_STREAM_H

#include <fstream>
#include <cstdint>
#include <cstddef>

class MiniStream {
protected:
    uint8_t _mode;
    
public:
    enum Mode
    {
        Write = 0,
        Read = 1,
        Append = 2,
        CRW = 3,
        ORW = 4
    };

    enum Pos
    {
        Set = 0,
        Cur = 1,
        End = 2
    };

    virtual ~MiniStream() = default;
    virtual bool open(const char* filename, uint8_t mode) = 0;
    virtual bool close() = 0;
    virtual bool isOpen() const = 0;
    virtual bool write(const void* buffer, size_t size, size_t count) = 0;
    virtual bool read(void* buffer, size_t size, size_t count) = 0;
    virtual bool seek(size_t offset, uint8_t origin) = 0;
    virtual size_t getPos() const = 0;
    virtual bool setPos(size_t t) = 0;
    virtual size_t size() const = 0;
    
    uint8_t getMode() { return _mode; }
};

class IStream : public MiniStream {
private:
    mutable std::fstream file_;

public:
    IStream(const char* filename, uint8_t mode) {
        open(filename, mode);
    }

    IStream() = default;

    ~IStream() override {
        close();
    }

    bool open(const char* filename, uint8_t mode) override {
        _mode = mode;
        std::ios_base::openmode fmode = std::ios_base::binary;

        switch (mode) {
        case MiniStream::Write:
            fmode |= std::ios_base::out;
            break;
        case MiniStream::Read:
            fmode |= std::ios_base::in;
            break;
        case MiniStream::Append:
            fmode |= std::ios_base::out | std::ios_base::app;
            break;
        case MiniStream::CRW:
            fmode |= std::ios_base::out | std::ios_base::in | std::ios_base::trunc;
            break;
        case MiniStream::ORW:
            fmode |= std::ios_base::out | std::ios_base::in;
            break;
        default:
            return false;
        }

        file_.open(filename, fmode);
        return file_.is_open();
    }

    bool close() override {
        if (file_.is_open()) {
            file_.close();
            return true;
        }
        return false;
    }

    bool isOpen() const override {
        return file_.is_open();
    }

    bool write(const void* buffer, size_t size, size_t count) override {
        if (!file_.is_open()) return false;
        file_.write(static_cast<const char*>(buffer), size * count);
        return !file_.fail();
    }

    bool read(void* buffer, size_t size, size_t count) override {
        if (!file_.is_open()) return false;
        file_.read(static_cast<char*>(buffer), size * count);
        return !file_.fail();
    }

    bool seek(size_t offset, uint8_t origin) override {
        if (!file_.is_open()) return false;

        std::ios_base::seekdir dir;
        switch (origin) {
        case MiniStream::Set:
            dir = std::ios_base::beg;
            break;
        case MiniStream::Cur:
            dir = std::ios_base::cur;
            break;
        case MiniStream::End:
            dir = std::ios_base::end;
            break;
        default:
            return false;
        }

        file_.seekg(offset, dir);
        return !file_.fail();
    }

    size_t getPos() const override {
        if (!file_.is_open()) return 0;
        return file_.tellg();
    }

    bool setPos(size_t pos) override {
        if (!file_.is_open()) return false;
        file_.seekg(pos);
        file_.seekp(pos);
        return !file_.fail();
    }

    size_t size() const override {
        if (!file_.is_open()) return 0;

        auto currentPos = file_.tellg();
        file_.seekg(0, std::ios_base::end);
        auto fileSize = file_.tellg();
        file_.seekg(currentPos);
        return fileSize;
    }
};

#endif // MINI_STREAM_H