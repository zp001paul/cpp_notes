package main

import "fmt"

func main() {
	const LOOPCNT = 100000

	startChan := [2]chan struct{}{
		make(chan struct{}),
		make(chan struct{}),
	}

	finishChan := make(chan struct{}, 2)

	var x, y, a, b int64

	// goroutine racer1
	go func() {
		for i := 0; i < LOOPCNT; i++ {
			<-startChan[0]
			x = 1
			b = y
			finishChan <- struct{}{}
		}
	}()

	// goroutine racer2
	go func() {
		for i := 0; i < LOOPCNT; i++ {
			<-startChan[1]
			y = 1
			a = x
			finishChan <- struct{}{}
		}
	}()

	// goroutine checker
	for i := 0; i < LOOPCNT; i++ {
		x, y = 0, 0
		startChan[0] <- struct{}{}
		startChan[1] <- struct{}{}
		<-finishChan
		<-finishChan
		if a == 0 && b == 0 {
			fmt.Printf("Loop: %d reorder!\n", i)
		}
	}
}
