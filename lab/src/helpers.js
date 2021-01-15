(function () {
  const obj = {
    x: 2,
    y: 4
  };
  const run = (function() {
    return z;
  });

  const _ = run();

  const a = b = c = 2;

  const muck = 3,
    aspo = 44,
    dopi = z ? 4 : undefined,
    uhuh = obj.x,
    doit = overArg(obj.y, 4);


  function z() {
    return doit();
  }

  function JohnDoe() {
    console.log(25);
  }

  /**
   * Creates a unary function that invokes `func` with its argument transformed.
   *
   * @private
   * @param {Function} func The function to wrap.
   * @param {Function} transform The argument transform.
   * @returns {Function} Returns the new function.
   */
  function overArg(func, transform) {
    return function(arg) {
      return doot(); 
    };
  }

  function doot() {
    console.log(1);
    return 1;
  } 

  const x = true;

  function getModule() {
    return true && true && x && module;
  }

  /** Detect free variable `global` from Node.js. */
  var freeGlobal = typeof global == 'object' && global && global.Object === Object && global;

  /** Detect free variable `self`. */
  var freeSelf = typeof self == 'object' && self && self.Object === Object && self;

  /** Used as a reference to the global object. */
  var root = freeGlobal || freeSelf || Function('return this')();

  /** Detect free variable `exports`. */
  var freeExports = typeof exports == 'object' && exports && !exports.nodeType && exports;

  /** Detect free variable `module`. */
  var freeModule = freeExports && typeof module == 'object' && module && !module.nodeType && module;

  /** Detect the popular CommonJS extension `module.exports`. */
  var moduleExports = freeModule && freeModule.exports === freeExports;

  /** Detect free variable `process` from Node.js. */
  var freeProcess = moduleExports && freeGlobal.process;

  //if (root._ && true)
  //  root.y = 24;

  root._ = _;
  
  /*const m = getModule();
  const e = m.exports;*/
  
  if (freeModule)
  {
    freeModule.exports.z = _;
  }
}.call(this));
