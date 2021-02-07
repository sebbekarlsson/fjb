import { useState, useMemo } from "react";

const style = {
  width: "250px",
  height: "250px",
  backgroundColor: "white",
  borderStyle: "solid",
  borderWidth: "1px",
  borderColor: "lightgray",
  boxSizing: "border-box",
  padding: "1rem",
  display: "flex",
  justifyContent: "center",
  alignItems: "center",
};

const toggledStyle = {
  backgroundColor: "lightgray",
};

export const Card = (props) => {
  const [toggled, setToggled] = useState(false);
  const handleClick = (e) => setToggled(!toggled);

  const mystyle = useMemo(() => {
    return toggled ? { ...style, ...toggledStyle } : style;
  }, [toggled]);

  return (
    <div onClick={handleClick} style={mystyle}>
      <p>{props.text}</p>
    </div>
  );
};
