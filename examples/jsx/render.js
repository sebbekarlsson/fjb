export function render(jsx, root_id) {
  const root = document.getElementById(root_id);
  root.innerHTML = "";

  if (Array.isArray(jsx)) {
    jsx.forEach((el) => root.appendChild(el));
  } else {
    root.appendChild(jsx);
  }
}
