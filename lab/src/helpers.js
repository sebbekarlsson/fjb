(function () {
  function bepo() {

  }

  function getProp(value) {
    return value != 0;
  }

  function z() {
    const x = 3;
    const value = getProp(x);
  } 

  function getModule() {
    return module;
  }

  const m = getModule();
  const e = m.exports;
  //module.exports.z = z;
  
  e.z = z;
}.call(this));
