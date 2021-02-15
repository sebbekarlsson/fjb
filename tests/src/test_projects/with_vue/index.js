import Vue from 'vue';

var app = new Vue({
  render(h)
  {
    return h('div', this.hi)
  }
});

console.log(app);
