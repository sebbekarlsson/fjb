function globalDef(value) {
  return window && value && value.name && (window[value.name] = value);
}
function __jsx_append_many(parent, elements) {
  for (var i = 0; i < elements.length; i++) {
    parent.appendChild(elements[i]);
  }
}
globalDef(__jsx_append_many);
function __jsx_append(parent, el) {
  if (!Array.isArray(el)) return parent.appendChild(el);
  return __jsx_append_many(parent, el);
}
globalDef(__jsx_append);
