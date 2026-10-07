function openDialogs() {
    let button = document.getElementById("addMealBtn");
    let mealDialog = document.getElementById("mealDialog");
    let caloriesDialog = document.getElementById("caloriesDialog");

    // Get button position
    let rect = button.getBoundingClientRect();

    // Position the meal dialog next to the button
    mealDialog.style.top = rect.top + window.scrollY + "px";
    mealDialog.style.left = rect.left + button.offsetWidth + 10 + "px";
    mealDialog.style.display = "block";

    // Position the calories dialog next to the meal dialog
    caloriesDialog.style.top = rect.top + window.scrollY + "px";
    caloriesDialog.style.left = rect.left + button.offsetWidth + mealDialog.offsetWidth + 20 + "px";
    caloriesDialog.style.display = "block";
}

function submitForms() {
    let mealName = document.getElementById("mealName").value;
    let mealCalories = document.getElementById("mealCalories").value;

    if (mealName && mealCalories) {
        alert("Meal Added:\n" + "Name: " + mealName + "\nCalories: " + mealCalories);
        
        // Close dialogs after submission
        document.getElementById("mealDialog").style.display = "none";
        document.getElementById("caloriesDialog").style.display = "none";
    } else {
        alert("Please fill out both fields!");
    }
}




function openDialogsM() {
    let button = document.getElementById("addMedBtn");
    let mealDialog = document.getElementById("medDialog");
    let caloriesDialog = document.getElementById("doseDialog");
    let caloriesDialog2 = document.getElementById("freqDialog");


    // Get button position
    let rect = button.getBoundingClientRect();

    // Position the meal dialog next to the button
    mealDialog.style.top = rect.top + window.scrollY + "px";
    mealDialog.style.left = rect.left + button.offsetWidth + 10 + "px";
    mealDialog.style.display = "block";

    // Position the calories dialog next to the meal dialog
    caloriesDialog.style.top = rect.top + window.scrollY + "px";
    caloriesDialog.style.left = rect.left + button.offsetWidth + mealDialog.offsetWidth + 20 + "px";
    caloriesDialog.style.display = "block";

    caloriesDialog2.style.top = rect.top + window.scrollY + "px";
    caloriesDialog2.style.left = rect.left + button.offsetWidth + mealDialog.offsetWidth + 230 + "px";
    caloriesDialog2.style.display = "block";
}

function submitFormsM() {
    let mealName = document.getElementById("medName").value;
    let mealCalories = document.getElementById("dosage").value;
    let mealCalories2 = document.getElementById("frequency").value;


    if (mealName && mealCalories && mealCalories2) {
        alert("Medicine:\n" + "Name: " + mealName + "\nDosage: " + mealCalories+ "\nFrequency"+ mealCalories2);
        
        // Close dialogs after submission
        document.getElementById("medDialog").style.display = "none";
        document.getElementById("doseDialog").style.display = "none";
        document.getElementById("freqDialog").style.display = "none";

    } else {
        alert("Please fill out all fields!");
    }
}





















document.addEventListener("DOMContentLoaded", function () {
    var xValues = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"];
    var stepsData = [5500, 6900, 9900, 7500, 8700, 6400, 12000];  // Steps per day
    var caloriesData = [2750, 3450, 4950, 3750, 4350, 3200, 6000];  // Calories burned per day
    

    new Chart(document.getElementById("myChart"), {
        type: "bar",
        data: {
            labels: xValues,
            datasets: [
                {
                    label: "Steps Taken",
                    backgroundColor: "rgb(57,255,20)", // Neon Green
                    data: stepsData,
                    barPercentage: 0.5, // Adjusts the width of individual bars
                    categoryPercentage: 0.15// Adjusts the spacing between bars
                   
                },
                {
                    label: "Calories Burned",
                    backgroundColor: "rgb(255,95,31)", // Soft Red
                    data: caloriesData,
                    barPercentage: 0.5, // Adjusts the width of individual bars
                    categoryPercentage: 0.15// Adjusts the spacing between bars
                   
                }
            ]
        },
        options: {
            responsive: true,
         
            plugins: {
                legend: { display: false} // Show legend for both datasets
            },
            title: {
                display: true,
                text: "Steps Taken vs. Calories Burned"
            },
            scales: {
                x: {
                    ticks: { autoSkip: false },
                    grid: { display: false }
                },
                y: {
                    beginAtZero: true
                }
            }
        }
    });
});



