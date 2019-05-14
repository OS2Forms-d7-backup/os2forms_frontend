/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


jQuery(document).ajaxComplete(function(e) {
  function dropdownWidth(){
    if (jQuery('.dropdown') != undefined){
      var width = jQuery('.dropdown').parent('.form-item').find('.input-group').width();
      var input_offset = jQuery('.dropdown').parent('.form-item').find('.input-group').offset();
      var dropdown_offset = jQuery('.dropdown').offset();
      jQuery('.dropdown').width(width);
      jQuery('.dropdown').offset({top:dropdown_offset.top, left: input_offset.left});
    }
  }
  dropdownWidth();
  jQuery(window).resize(function(e) {
    dropdownWidth();
  });
});