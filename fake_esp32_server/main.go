package main

import (
	"fmt"
	"math/rand"
	"net/http"
)

func handleGoCommand(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Yes")
}

func handleHealthCommand(w http.ResponseWriter, r *http.Request) {
	if rand.Intn(5) == 1 {
		fmt.Fprintf(w, "I'm deaded")
	} else {
		fmt.Fprintf(w, "Healthy")
	}
}

func main() {
	http.HandleFunc("/go", handleGoCommand)
	http.HandleFunc("/health", handleHealthCommand)

	if err := http.ListenAndServe(":80", nil); err != nil {
		panic(err)
	}
}
