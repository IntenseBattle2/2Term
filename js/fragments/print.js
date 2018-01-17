//Print fragment for 2Term - displays text
//(C)2018 Intense Battle 2
/*
 * This file is part of the program 2Term.
 *
 * 2Term is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * 2Term is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//You can contact the authors of this program at either <ib2member01@gmail.com> or <ib2member02@gmail.com>

function print(s){
  alert(typeof s);
  if(typeof s != "string"){ console.error("print: passed argument is not a string.");return -1;}
  document.body.innerHTML+=s;
}
