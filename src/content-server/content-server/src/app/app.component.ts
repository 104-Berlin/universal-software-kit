import { Component } from '@angular/core';
import {HttpClient} from "@angular/common/http"

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'content-server';

  constructor(private http: HttpClient)
  {}
  
  onFileSelected(event : any) {
    let file : File = event.target.files[0]
    if (file) {
      console.log("File upload", file)
      let formData = new FormData()
      formData.append("File", file)
      this.http.post("/api/application", formData).subscribe()
    }
  }
}
