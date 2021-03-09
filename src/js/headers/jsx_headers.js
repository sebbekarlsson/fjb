function globalDef(value) {
  return window && value && value.name && (window[value.name] = value);
}

var __jsx_queue = [];
globalDef(__jsx_queue);

function __isElement(obj) {
  try {
    return obj instanceof HTMLElement;
  } catch (e) {
    return (
      typeof obj === "object" &&
      obj.nodeType === 1 &&
      typeof obj.style === "object" &&
      typeof obj.ownerDocument === "object"
    );
  }
}
globalDef(__isElement);

function __toElement(val) {
  if (__isElement(val)) return val;

  if (typeof val == "string" || typeof val == "number")
    return document.createTextNode(val);
  
  return val;
}

function __jsx_append_many(parent, elements) {
  for (var i = 0; i < elements.length; i++) {
    parent.appendChild(__toElement(elements[i]));
  }
}
globalDef(__jsx_append_many);
function __jsx_append(parent, el) {
  if (!Array.isArray(el)) return parent.appendChild(__toElement(el));
  return __jsx_append_many(parent, el);
}
globalDef(__jsx_append);

function React_createTextNode(text) {
  return React.createElement("span", null, text);
}
globalDef(React_createTextNode);
