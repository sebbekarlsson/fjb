export function render(jsx, root_id) {
  const el = document.getElementById(root_id);
  el.innerHTML = '';
  el.appendChild(jsx);
}
