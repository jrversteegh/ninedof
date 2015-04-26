/* vim: set sw=2 ts=2 et: */

extern "C" {
  #include <linux/i2c-dev.h>
  #include <byteswap.h>
  #include <endian.h>
  #include <fcntl.h>
}

#include <cerrno>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <map>
#include <iostream>

#include "../include/errors.h"
#include "../include/i2cbus.h"

namespace ninedof {

struct BusRef {
  int refcount;
  int address;
  int file;
  BusRef(): refcount(), address(), file() {}
  BusRef(const int filehandle): refcount(1), address(), file(filehandle) {}
  BusRef(const BusRef& busref): refcount(busref.refcount), address(busref.address), file(busref.file) {}
  const BusRef& operator=(const BusRef& busref) {
    refcount = busref.refcount;
    address = busref.address;
    file = busref.file;
    return *this;
  }
  ~BusRef() {}
};

typedef std::map<int, BusRef> BusRefs;
typedef BusRefs::iterator BusRef_i;

static BusRefs busrefs;

void I2CBus::open_bus_()
{
  BusRef_i i = busrefs.find(busno_);
  if (i != busrefs.end()) {
    file_ = i->second.file;
    ++i->second.refcount;
  } else {
    char filename[16];
	  snprintf(filename, 15, "/dev/i2c-%d", busno_);
	  file_ = open(filename, O_RDWR);
	  if (file_ < 0) {
	    throw Error("Failed to open I2C bus.", errno);
	  }
    busrefs[busno_] = BusRef(file_); 
  }
}

void I2CBus::close_bus_()
{
  BusRef_i i = busrefs.find(busno_);
  if (i != busrefs.end()) {
    --i->second.refcount;
    if (i->second.refcount <= 0) {
      close(i->second.file);
      i = busrefs.erase(i);
    }
  } 
}

void I2CBus::select_address(const int address)
{
  BusRef_i i = busrefs.find(busno_);
  if (i != busrefs.end()) {
    if (i->second.address != address) {
      if (ioctl(file_, I2C_SLAVE, address) < 0) {
        throw Error("Failed to select I2C address.", errno);
      }
      i->second.address = address;
    }
  }
  else {
    throw Error("Bus not open", busno_);
  }
}

const int I2CBus::get_selected_address() const
{
  BusRef_i i = busrefs.find(busno_);
  if (i != busrefs.end()) {
    return i->second.address; 
  } else {
    throw Error("Bus not open", busno_);
  }
}

Ints I2CBus::scan() {
  Ints result;
  for (int i = 1; i < 256; ++i) {
    try {
      select_address(i);
      if (i2c_smbus_read_byte(file_) >= 0) {
        result.push_back(i);  
      }
    } 
    catch(Error) {
    }
  }
  return result;
}

void I2CDevice::write_byte(const int offset, const Byte value) const
{
  select_();
  __s32 result = i2c_smbus_write_byte_data(bus_.get_file(), offset, value);
  if (result < 0) {
    throw Error("Failed to write I2C data.", errno);
  }
}	

void I2CDevice::write_bytes(const int offset, const Bytes& values) const
{
  select_();
  __s32 result = i2c_smbus_write_i2c_block_data(bus_.get_file(), offset, values.size(), values.data());
  if (result < 0) {
    throw Error("Failed to write I2C data.", errno);
  }
}	

Byte I2CDevice::read_byte(const int offset) const
{
  select_();
  __s32 result = i2c_smbus_read_byte_data(bus_.get_file(), offset);
  if (result < 0) {
    throw Error("Failed to read I2C data.", errno);
  }
  return 0xFF & result;
}	

Bytes I2CDevice::read_bytes(const int offset, const int count) const
{
  select_();
  Bytes bytes(count);
  __s32 result = i2c_smbus_read_i2c_block_data(bus_.get_file(), offset & 0xFF, count & 0xFF, bytes.data());
  if (result < count) {
    throw Error("Failed to read I2C data.", errno);
  }
  return bytes;
}	

void I2CDevice::write_word(const int offset, const Word value) const
{
  select_();
  Word word = value;
  if (!little_endian_) {
    word = bswap_16(word);
  }
  __s32 result = i2c_smbus_write_word_data(bus_.get_file(), offset, word);
  if (result < 0) {
    throw Error("Failed to write I2C data.", errno);
  }
}	

void I2CDevice::write_words(const int offset, const Words& values) const
{
  select_();
  Words words(values);
  for (Word_i i = words.begin(); i != words.end(); ++i) {
    if (little_endian_) {
      *i = htole16(*i);
    } else {
      *i = htobe16(*i);
    }
  }
  __s32 result = i2c_smbus_write_i2c_block_data(bus_.get_file(), offset, words.size() * 2, 
    reinterpret_cast<Byte*>(words.data()));
  if (result < 0) {
    throw Error("Failed to write I2C data.", errno);
  }
}	

Word I2CDevice::read_word(const int offset) const
{
  select_();
  __s32 result = i2c_smbus_read_word_data(bus_.get_file(), offset);
  if (result < 0) {
    throw Error("Failed to read I2C data.", errno);
  }
  Word word = 0xFFFF & result;
  if (!little_endian_)
    word = bswap_16(word);
  return word; 
}	

Words I2CDevice::read_words(const int offset, const int count) const
{
  select_();
  Words words(count);
  __s32 result = i2c_smbus_read_i2c_block_data(bus_.get_file(), offset, count * 2, 
    reinterpret_cast<Byte*>(words.data()));
  if (result < (count * 2)) {
    throw Error("Failed to read I2C data.", errno);
  }
  for (Word_i i = words.begin(); i != words.end(); ++i) {
    if (little_endian_) {
      *i = le16toh(*i);
    } else {
      *i = be16toh(*i);
    }
  }
  return words;
}	

}  // namespace ninedof