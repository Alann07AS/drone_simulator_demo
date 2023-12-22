function saveData() {
    localStorage.setItem("data", JSON.stringify(data));
}
console.log(localStorage.getItem("data")||"{}");
const data = JSON.parse(localStorage.getItem("data")||"{}")
console.log(data);