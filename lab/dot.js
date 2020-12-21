const password = (function () {
  let exports = {};
  let module = { exports: exports };
  ("use strict");
  var random = (function () {
    let exports = {};
    let module = { exports: exports };
    ("use strict");
    var secureRandom = (function () {
      let exports = {};
      let module = { exports: exports };
      !(function (globals) {
        "use strict";
        if (typeof define !== "undefined" && define.amd) {
          define([], function () {
            return secureRandom;
          });
        } else if (typeof module !== "undefined" && module.exports) {
          module.exports = secureRandom;
        } else {
          globals.secureRandom = secureRandom;
        }
        function secureRandom(count, options) {
          options = options || {
            type: "Array",
          };
          if (
            typeof process != "undefined" &&
            typeof process.pid == "number" &&
            process.versions &&
            process.versions.node
          ) {
            return nodeRandom(count, options);
          } else {
            var crypto = window.crypto || window.msCrypto;
            if (!crypto) {
              throw new Error("Your browser does not support window.crypto.");
            }
            return browserRandom(count, options);
          }
        }
        function nodeRandom(count, options) {
          var crypto = require("crypto");
          var buf = crypto.randomBytes(count);
          switch (options.type) {
            case "Array":
              return [].slice.call(buf);
            case "Buffer":
              return buf;
            case "Uint8Array":
              var arr = new Uint8Array(count);
              for (var i = 0; i < count; ii) {
                arr[i] = buf.readUInt8(i);
              }
              return arr;
            default:
              throw new Error(options.type + " is unsupported.");
          }
        }
        function browserRandom(count, options) {
          var nativeArr = new Uint8Array(count);
          var crypto = window.crypto || window.msCrypto;
          crypto.getRandomValues(nativeArr);
          switch (options.type) {
            case "Array":
              return [].slice.call(nativeArr);
            case "Buffer":
              try {
                var b = new Buffer(1);
              } catch (e) {
                throw new Error(
                  "Buffer not supported in this environment. Use Node.js or Browserify for browser support."
                );
              }
              return new Buffer(nativeArr);
            case "Uint8Array":
              return nativeArr;
            default:
              throw new Error(options.type + " is unsupported.");
          }
        }
        secureRandom.randomArray = function (byteCount) {
          return secureRandom(byteCount, {
            type: "Array",
          });
        };
        secureRandom.randomUint8Array = function (byteCount) {
          return secureRandom(byteCount, {
            type: "Uint8Array",
          });
        };
        secureRandom.randomBuffer = function (byteCount) {
          return secureRandom(byteCount, {
            type: "Buffer",
          });
        };
      })(this);

      /* noop */
      return module && module.exports ? module.exports : exports;
    })();
    var util = (function () {
      let exports = {};
      let module = { exports: exports };
      ("use strict");
      function assign() {
        var objects = toArray(arguments);
        var target = objects[0];
        var sources = objects.slice(1);
        sources
          .filter(function (source) {
            return !!source;
          })
          .forEach(function (source) {
            Object.keys(source).forEach(function (key) {
              target[key] = source[key];
            });
          });
        return target;
      }
      exports.assign = assign;
      function intersection(left, right) {
        left = toArray(left);
        right = toArray(right);
        var leftSet = {};
        left.forEach(function (k) {
          leftSet[k] = true;
        });
        var rightSet = {};
        right.forEach(function (k) {
          rightSet[k] = true;
        });
        return Object.keys(leftSet).filter(function (k) {
          return Object.prototype.hasOwnProperty.call(rightSet, k);
        });
      }
      exports.intersection = intersection;
      function isInteger(n) {
        return typeof n === "number" && isFinite(n) && Math.floor(n) === n;
      }
      exports.isInteger = isInteger;
      function isString(s) {
        return typeof s === "string" || s instanceof String;
      }
      exports.isString = isString;
      function range(n) {
        return Array.apply(null, Array(n)).map(function (x, i) {
          return i;
        });
      }
      exports.range = range;
      function repeat(val, n) {
        return range(n).map(function () {
          return val;
        });
      }
      exports.repeat = repeat;
      function toArray(x) {
        return Array.prototype.slice.call(x);
      }
      exports.toArray = toArray;

      /* noop */

      return module && module.exports ? module.exports : exports;
    })();
    function Random(randomSource) {
      if (typeof randomSource !== "function") {
        throw new Error("Must pass a randomSource function");
      }
      this._randomSource = randomSource;
    }
    Random.prototype.choose = function (choices) {
      if (!choices || !choices.length) {
        throw new Error("Must pass 1 or more choices");
      }
      return choices[this._getInt(choices.length)];
    };
    Random.prototype.getInt = function (upperBoundExclusive) {
      if (upperBoundExclusive === undefined) {
        throw new Error("Must pass an upper bound");
      }
      if (!util.isInteger(upperBoundExclusive)) {
        throw new Error("Upper bound must be a number");
      }
      if (upperBoundExclusive < 1) {
        throw new Error("Upper bound must be > 0");
      }
      return this._getInt(upperBoundExclusive);
    };
    Random.prototype._getInt = function (upperBoundExclusive) {
      if (upperBoundExclusive === 1) {
        return 0;
      }
      var numBytes = Math.ceil(Math.log(upperBoundExclusive) / Math.log(256));
      var startOfBias =
        Math.pow(2, 8 * numBytes) -
        (Math.pow(2, 8 * numBytes) % upperBoundExclusive);
      var randomNumber;
      do {
        randomNumber = byteArrayToInt(this._randomSource(numBytes));
      } while (randomNumber >= startOfBias);
      return randomNumber % upperBoundExclusive;
    };
    Random.prototype.shuffle = function (items) {
      items = Array.prototype.slice.call(items || []);
      var result = [];
      while (items.length) {
        result.push(items.splice(this._getInt(items.length), 1)[0]);
      }
      return result;
    };
    exports.Random = Random;
    exports.default = new Random(secureRandom);
    function byteArrayToInt(bytes) {
      bytes = bytes || [];
      var result = 0;
      var power = 1;
      for (var i = bytes.length - 1; i >= 0; i--) {
        result += bytes[i] * power;
        power *= 256;
      }
      return result;
    }

    /* noop */

    return module && module.exports ? module.exports : exports;
  })().default;
  var sets = (function () {
    let exports = {};
    let module = { exports: exports };
    exports.lower = "abcdefghijklmnopqrstuvwxyz";
    exports.upper = exports.lower.toUpperCase();
    exports.consonants = "bcdfghjklmnpqrstvwxyz";
    exports.vowels = "aeiou";
    exports.digits = "1234567890";
    exports.symbols = "!@#$%^&*()";
    exports.fullSymbols = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    exports.copyableSymbols = "_";

    /* noop */

    return module && module.exports ? module.exports : exports;
  })();
  var util = (function () {
    let exports = {};
    let module = { exports: exports };
    ("use strict");
    function assign() {
      var objects = toArray(arguments);
      var target = objects[0];
      var sources = objects.slice(1);
      sources
        .filter(function (source) {
          return !!source;
        })
        .forEach(function (source) {
          Object.keys(source).forEach(function (key) {
            target[key] = source[key];
          });
        });
      return target;
    }
    exports.assign = assign;
    function intersection(left, right) {
      left = toArray(left);
      right = toArray(right);
      var leftSet = {};
      left.forEach(function (k) {
        leftSet[k] = true;
      });
      var rightSet = {};
      right.forEach(function (k) {
        rightSet[k] = true;
      });
      return Object.keys(leftSet).filter(function (k) {
        return Object.prototype.hasOwnProperty.call(rightSet, k);
      });
    }
    exports.intersection = intersection;
    function isInteger(n) {
      return typeof n === "number" && isFinite(n) && Math.floor(n) === n;
    }
    exports.isInteger = isInteger;
    function isString(s) {
      return typeof s === "string" || s instanceof String;
    }
    exports.isString = isString;
    function range(n) {
      return Array.apply(null, Array(n)).map(function (x, i) {
        return i;
      });
    }
    exports.range = range;
    function repeat(val, n) {
      return range(n).map(function () {
        return val;
      });
    }
    exports.repeat = repeat;
    function toArray(x) {
      return Array.prototype.slice.call(x);
    }
    exports.toArray = toArray;

    /* noop */

    return module && module.exports ? module.exports : exports;
  })();
  Object.keys(sets).forEach(function (k) {
    exports[k] = sets[k];
  });
  var defaultAmbiguousSets = ["Il1|", "O0"];
  function randomPassword(opts) {
    var defaults = {
      avoidAmbiguous: true,
      characters: [
        sets.lower,
        {
          characters: sets.upper,
          exactly: 1,
        },
        {
          characters: sets.symbols,
          exactly: 1,
        },
      ],
      length: 12,
      predicate: function () {
        return true;
      },
      random: random,
    };
    opts = util.assign({}, defaults, opts);
    var characterRules = translateRules(opts);
    if (!util.isInteger(opts.length)) {
      throw new Error("length must be an integer");
    }
    if (opts.length < 1) {
      throw new Error("length must be > 0");
    }
    if (opts.length < characterRules.length) {
      throw new Error("length must be >= # of character sets passed");
    }
    if (
      characterRules.some(function (rule) {
        return !rule.characters;
      })
    ) {
      throw new Error("No character set may be empty");
    }
    if (characterRules.length === 0) {
      throw new Error("Must pass one or more character sets");
    }
    if (typeof opts.predicate !== "function") {
      throw new Error("predicate must be a function");
    }
    var minimumLength = characterRules
      .map(function (rule) {
        return rule.exactly || 1;
      })
      .reduce(function (l, r) {
        return l + r;
      }, 0);
    if (opts.length < minimumLength) {
      throw new Error("length is too short for character set rules");
    }
    var allExactly = characterRules.every(function (rule) {
      return rule.exactly;
    });
    if (allExactly) {
      var exactlyLength = characterRules.reduce(function (l, r) {
        return l.exactly + r.exactly;
      }, 0);
      if (exactlyLength !== opts.length) {
        throw new Error(
          "Must pass a set without exactly rule to generate the specified length"
        );
      }
    }
    var result;
    do {
      result = generatePassword(characterRules, opts.length, opts.random);
    } while (!opts.predicate(result));
    return result;
  }
  exports.randomPassword = randomPassword;
  function generatePassword(characterRules, length, random) {
    var requiredSetsWithRepeats = characterRules
      .map(function (rule) {
        return rule.exactly
          ? util.repeat(rule.characters, rule.exactly)
          : [rule.characters];
      })
      .reduce(function (l, r) {
        return l.concat(r);
      });
    var requiredChoices = requiredSetsWithRepeats.map(function (characters) {
      return random.choose(characters);
    });
    var fillCharcters = characterRules
      .filter(function (rule) {
        return !rule.exactly;
      })
      .map(function (rule) {
        return rule.characters;
      })
      .join("");
    var randomChoices = util
      .range(length - requiredChoices.length)
      .map(function () {
        return random.choose(fillCharcters);
      });
    var shuffled = random.shuffle(requiredChoices.concat(randomChoices));
    return shuffled.join("");
  }
  function translateRules(opts) {
    if (!opts.characters) {
      return [];
    }
    var result = Array.isArray(opts.characters)
      ? opts.characters
      : [opts.characters];
    result = result.map(function (x) {
      return util.isString(x)
        ? {
            characters: x,
          }
        : x;
    });
    var ambiguousSets =
      opts.avoidAmbiguous === true
        ? defaultAmbiguousSets
        : opts.avoidAmbiguous || [];
    stripAmbiguous(result, ambiguousSets);
    return result;
  }
  function stripAmbiguous(characterRules, ambiguousSets) {
    var allCharacters = characterRules
      .map(function (rule) {
        return rule.characters;
      })
      .join("");
    var ambiguousCharacters = ambiguousSets
      .filter(function (ambiguousSet) {
        return util.intersection(ambiguousSet, allCharacters).length > 1;
      })
      .join("");
    characterRules.forEach(function (rule) {
      rule.characters = util
        .toArray(rule.characters)
        .filter(function (ch) {
          return ambiguousCharacters.indexOf(ch) < 0;
        })
        .join("");
    });
  }
  function randomString(opts) {
    var defaults = {
      characters: [sets.lower, sets.upper, sets.digits, sets.symbols],
      length: 20,
    };
    opts = util.assign({}, defaults, opts);
    opts.avoidAmbiguous = false;
    return randomPassword(opts);
  }
  exports.randomString = randomString;

  /* noop */

  return module && module.exports ? module.exports : exports;
})();
console.log(
  password.randomPassword({
    characters: password.lower,
    length: 4,
  })
);
const x = 3 * 4;
if (x > 3) {
  console.log(x);
} else {
  console.log("hello");
}
const main = () => {
  doSomething(x);
};

/* noop */
