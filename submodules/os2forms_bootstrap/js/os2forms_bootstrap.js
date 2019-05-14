/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


jQuery(document).ajaxComplete(function(e) {
  function dropdownWidth(){
    var focused = jQuery(':focus');
    var width = focused.closest('.input-group').width();
    var input_offset = focused.closest('.input-group').offset();
    var input_height = focused.closest('.input-group').height();
    jQuery('.dropdown').width(width);
    if (input_offset){
      top_offset = input_offset.top + input_height;
      jQuery('.dropdown').offset({top:top_offset, left: input_offset.left});
    }
  }
  dropdownWidth();
});
