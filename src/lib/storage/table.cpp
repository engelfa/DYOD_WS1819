#include "table.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "value_segment.hpp"

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

Table::Table(const uint32_t chunk_size) : _chunk_size(chunk_size) { _chunks.push_back(std::make_shared<Chunk>()); }

void Table::add_column(const std::string& name, const std::string& type) {
  _column_names.push_back(name);
  _types.push_back(type);
  for (uint32_t i = 0; i < chunk_count(); ++i) {
    _chunks.at(i)->add_segment(make_shared_by_data_type<BaseSegment, ValueSegment>(type));
  }
}

void Table::append(std::vector<AllTypeVariant> values) {
  if (_chunks.at(chunk_count() - 1)->size() >= chunk_size()) {
    _chunks.push_back(std::make_shared<Chunk>());
    for (uint32_t i = 0; i < _column_names.size(); ++i) {
      _chunks.at(chunk_count() - 1)->add_segment(make_shared_by_data_type<BaseSegment, ValueSegment>(_types.at(i)));
    }
  }
  _chunks.at(chunk_count() - 1)->append(values);
}

uint16_t Table::column_count() const { return _chunks.at(0)->column_count(); }

uint64_t Table::row_count() const { return (chunk_count() - 1) * _chunk_size + _chunks.at(chunk_count() - 1)->size(); }

ChunkID Table::chunk_count() const { return ChunkID{_chunks.size()}; }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  for (int i = 0; i < column_count(); ++i) {
    if (_column_names.at(i) == column_name) {
      return ColumnID{i};
    }
  }
  throw std::exception();
  return ColumnID{0};
}

uint32_t Table::chunk_size() const { return _chunk_size; }

const std::vector<std::string>& Table::column_names() const { return _column_names; }

const std::string& Table::column_name(ColumnID column_id) const { return _column_names.at(column_id); }

const std::string& Table::column_type(ColumnID column_id) const { return _types.at(column_id); }

Chunk& Table::get_chunk(ChunkID chunk_id) { return *_chunks.at(chunk_id); }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { return *_chunks.at(chunk_id); }

}  // namespace opossum
