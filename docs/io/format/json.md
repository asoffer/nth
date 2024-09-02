# `//nth/format:json`

This target provides a structural formatter that writes objects as valid
[JSON](https://www.json.org/). Containers such as `std::vector` or `std::array` are formatted as
JSON arrays. Set-like (unordered) containers are also formatted as JSON arrays. Associative
containers such as `absl::flat_hash_map` or `absl::btree_map` are formatted as JSON objects.
