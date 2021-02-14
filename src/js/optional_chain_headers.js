function _fjb_optional_chain(obj, key) {
  if ((obj || {}).hasOwnProperty(key)) return obj[key];
  return {};
}
