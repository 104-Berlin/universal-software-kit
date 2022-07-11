package main

import (
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
)

func main() {
	r := gin.Default()
	r.GET("/api", func(ctx *gin.Context) {
		ctx.String(http.StatusOK, "Hello World from GIN")
	})
	r.POST("/api/application", func(ctx *gin.Context) {
		file, err := ctx.FormFile("File")
		if err != nil {
			fmt.Println("Error uploading file: ", err)
			ctx.AbortWithStatus(http.StatusBadRequest)
		}
		fmt.Println("Uploading file: ", file.Filename)

		fileErr := ctx.SaveUploadedFile(file, "applications/MyApplication.afile")
		if fileErr != nil {
			fmt.Println("Error saving file: ", fileErr)
		}

		ctx.String(http.StatusOK, fmt.Sprintf("'%s' uploaded!", file.Filename))
	})
	r.Run(":5000")
}
