(function($){
	var $main_menu = $('ul.nav'),
		$featured_section = $('#featured_section'),
		$featured = $('#featured'),
		et_slider,
		et_video_slider,
		slider_arrows_speed = 300,
		video_hover_play_speed = 300,
		$featured_controls = $featured_section.find('#switcher li'),
		$video_slider = $('#video-content'),
		$video_control_item = $('#video-switcher li'),
		$comment_form = jQuery('form#commentform'),
		$tabbed_container = $('#all-tabs'),
		et_container_width = $('#main-area .container').innerWidth(),
		$top_menu = $('#top-menu > ul.nav'),
		$second_menu = $('#second-menu > ul.nav'),
		et_is_ie7 = $('html#ie7').length;

	$(document).ready(function(){
		$main_menu.superfish({
			delay:       300,                            // one second delay on mouseout
			animation:   {opacity:'show',height:'show'},  // fade-in and slide-down animation
			speed:       'fast',                          // faster animation speed
			autoArrows:  true,                           // disable generation of arrow mark-up
			dropShadows: false                            // disable drop shadows
		});

		$('article.post, article.page').fitVids();

		$('#left-area iframe').each( function(){
			// return if we're dealing with google ads iframe
			if ( typeof $(this).attr('src') == 'undefined' ) return;

			var $this_video = $(this),
				src_attr = $this_video.attr('src'),
				wmode_character = src_attr.indexOf( '?' ) == -1 ? '?' : '&amp;',
				this_src = src_attr + wmode_character + 'wmode=opaque';
			$this_video.attr('src',this_src);
		} );

		if ( $featured.length ){
			et_slider_settings = {
				slideshow: false,
				before: function(slider){
					$featured_controls.closest('#switcher').find('li').removeClass('active-slide').eq( slider.animatingTo ).addClass('active-slide');
				},
				start: function(slider) {
					et_slider = slider;
				}
			}

			if ( $featured.hasClass('et_slider_auto') ) {
				var et_slider_autospeed_class_value = /et_slider_speed_(\d+)/g;

				et_slider_settings.slideshow = true;

				et_slider_autospeed = et_slider_autospeed_class_value.exec( $featured.attr('class') );

				et_slider_settings.slideshowSpeed = et_slider_autospeed[1];
			}

			if ( $featured.hasClass('et_slider_effect_slide') ){
				et_slider_settings.animation = 'slide';
			}

			et_slider_settings.pauseOnHover = true;

			$featured.flexslider( et_slider_settings );
		}

		$('#featured_section, #video-slider-section').hover( function(){
			$(this).find( '.flex-direction-nav .flex-prev' ).css( { 'display' : 'block', 'opacity' : 0 } ).stop( true, true ).animate( { 'left' : '60px', 'opacity' : 1 }, slider_arrows_speed );
			$(this).find( '.flex-direction-nav .flex-next' ).css( { 'display' : 'block', 'opacity' : 0 } ).stop( true, true ).animate( { 'right' : '60px', 'opacity' : 1 }, slider_arrows_speed );
		}, function(){
			$(this).find( '.flex-direction-nav .flex-prev' ).stop( true, true ).animate( { 'left' : '0', 'opacity' : 0 }, slider_arrows_speed );
			$(this).find( '.flex-direction-nav .flex-next' ).stop( true, true ).animate( { 'right' : '0', 'opacity' : 0 }, slider_arrows_speed );
		} );

		$featured_controls.hover( function(){
			$(this).addClass( 'switcher_hover' );
		}, function(){
			$(this).removeClass( 'switcher_hover' );
		} );

		$featured_controls.click( function(){
			var $this_control = $(this),
				order = $this_control.prevAll('li').length;

			if ( $this_control.hasClass('active-slide') ) return;

			$this_control.closest('#switcher').find('li').removeClass( 'active-slide' );
			$this_control.addClass( 'active-slide' );

			$featured.flexslider( order );

			return false;
		} );

		$('#all-tabs li').hover( function(){
			$(this).addClass( 'et_tab_link_hover' );
		}, function(){
			$(this).removeClass( 'et_tab_link_hover' );
		} );

		if ( $video_slider.length ){
			// remove autoplay from all videos, except the first one, in the slider
			$video_slider.find( 'iframe' ).each( function( index ) {
				var $this_frame = $(this),
					frame_src	= $this_frame.attr( 'src' );

				// don't remove autoplay from the first video in the slider
				if ( index == 0 ) return;

				if ( -1 !== frame_src.indexOf( 'autoplay=1' ) ) {
					frame_src = frame_src.replace( /autoplay=1/g, '' );
					$this_frame
						.addClass( 'et_autoplay_removed' )
						.attr( 'src', '' )
						.attr( 'src', frame_src );
				}
			} );

			$video_slider.fitVids().flexslider( {
				slideshow: false,
				before: function(slider){
					$video_control_item.closest('#video-switcher').find('li').removeClass('active_video').eq( slider.animatingTo ).addClass('active_video');
					et_stop_video( slider );
				},
				start: function(slider) {
					et_video_slider = slider;
				}
			} );
		}

		$video_control_item.hover( function(){
			$(this).find('.et_video_play').css( { 'display' : 'block', 'opacity' : 0 } ).stop(true,true).animate( { opacity : 1 }, video_hover_play_speed );
		}, function(){
			$(this).find('.et_video_play').stop(true,true).animate( { opacity : 0 }, video_hover_play_speed );
		} );

		$video_control_item.click( function(){
			var $this_link = $(this),
				order = $this_link.prevAll('li').length;

			if ( $this_link.hasClass('active_video') ) return false;

			$this_link.closest('#video-switcher').find('li').removeClass('active_video');
			$this_link.addClass('active_video');

			$video_slider.flexslider( order );

			return false;
		} );

		et_search_bar();
		function et_search_bar(){
			var $searchform = $('#main-header #search-form'),
				$searchinput = $searchform.find("#searchinput"),
				searchvalue = $searchinput.val();

			$searchinput.focus(function(){
				if (jQuery(this).val() === searchvalue) jQuery(this).val("");
			}).blur(function(){
				if (jQuery(this).val() === "") jQuery(this).val(searchvalue);
			});
		}

		$comment_form.find('input:text, textarea').each(function(index,domEle){
			var $et_current_input = jQuery(domEle),
				$et_comment_label = $et_current_input.siblings('label'),
				et_comment_label_value = $et_current_input.siblings('label').text();
			if ( $et_comment_label.length ) {
				$et_comment_label.hide();
				if ( $et_current_input.siblings('span.required') ) {
					et_comment_label_value += $et_current_input.siblings('span.required').text();
					$et_current_input.siblings('span.required').hide();
				}
				$et_current_input.val(et_comment_label_value);
			}
		}).bind('focus',function(){
			var et_label_text = jQuery(this).siblings('label').text();
			if ( jQuery(this).siblings('span.required').length ) et_label_text += jQuery(this).siblings('span.required').text();
			if (jQuery(this).val() === et_label_text) jQuery(this).val("");
		}).bind('blur',function(){
			var et_label_text = jQuery(this).siblings('label').text();
			if ( jQuery(this).siblings('span.required').length ) et_label_text += jQuery(this).siblings('span.required').text();
			if (jQuery(this).val() === "") jQuery(this).val( et_label_text );
		});

		// remove placeholder text before form submission
		$comment_form.submit(function(){
			$comment_form.find('input:text, textarea').each(function(index,domEle){
				var $et_current_input = jQuery(domEle),
					$et_comment_label = $et_current_input.siblings('label'),
					et_comment_label_value = $et_current_input.siblings('label').text();

				if ( $et_comment_label.length && $et_comment_label.is(':hidden') ) {
					if ( $et_comment_label.text() == $et_current_input.val() )
						$et_current_input.val( '' );
				}
			});
		});

		if ( $('ul.et_disable_top_tier').length ) $("ul.et_disable_top_tier > li > ul").prev('a').attr('href','#');

		$('#top-menu > ul.nav > li, #second-menu > ul.nav > li').each( function(){
			var $this_li = $(this),
				li_text = $this_li.find('> a').html();

			$this_li.find('> a').html( '<span class="main_text">' + li_text + '</span>' + ( ! et_is_ie7 ? '<span class="menu_slide">' + li_text + '</span>' : '' ) );
		} );

		if ( ! et_is_ie7 ){
			$('#top-menu > ul.nav > li > a, #second-menu > ul.nav > li > a').hover(
				function(){
					$(this).find('span.main_text').css( { 'display' : 'block', 'opacity' : 0 } ).stop(true,true).animate( { 'marginTop' : '-59px', 'opacity' : 1 }, 400 );
				}, function(){
					$(this).find('span.main_text').stop(true,true).animate( { 'marginTop' : '0' }, 400 );
				}
			);
		}

		$( '#tab-controls a' ).click( function(){
			var $this_li = $(this).closest('li'),
				order = $this_li.prevAll('li').length,
				prev_active_order = $this_li.closest('ul').find('li.active').prevAll('li').length;

			if ( $this_li.hasClass('active') ) return false;

			$this_li.closest('ul').find('li').removeClass('active');
			$this_li.addClass('active');

			$tabbed_container.find('>div').eq(prev_active_order).animate( { opacity : 0 }, 500, function(){
				$(this).css( 'display', 'none' );
				$tabbed_container.find('>div').eq(order).css( { 'display' : 'block', 'opacity' : 0 } ).animate( { opacity : 1 }, 500 );
			} );

			return false;
		} );

		et_duplicate_menu( $('#top-menu ul.nav'), $('#main-header .mobile_nav'), 'mobile_menu', 'et_mobile_menu' );
		et_duplicate_menu( $('#second-menu ul.nav'), $('#secondary-menu .mobile_nav'), 'category_mobile_menu', 'et_mobile_menu' );

		function et_duplicate_menu( menu, append_to, menu_id, menu_class ){
			var $cloned_nav;

			menu.clone().attr('id',menu_id).removeClass().attr('class',menu_class).appendTo( append_to );
			$cloned_nav = append_to.find('> ul');
			$cloned_nav.find('.menu_slide').remove();
			$cloned_nav.find('li:first').addClass('et_first_mobile_item');

			append_to.click( function(){
				if ( $(this).hasClass('closed') ){
					$(this).removeClass( 'closed' ).addClass( 'opened' );
					$cloned_nav.slideDown( 500 );
				} else {
					$(this).removeClass( 'opened' ).addClass( 'closed' );
					$cloned_nav.slideUp( 500 );
				}
				return false;
			} );

			append_to.find('a').click( function(event){
				event.stopPropagation();
			} );
		}

		function et_stop_video( slider ){
			var $et_video, et_video_src;

			// if there is a video in the slide, stop it when switching to another slide
			if ( slider.find( '.slides li' ).eq( slider.currentSlide ).has( 'iframe' ) ){
				$et_video 		= slider.find( '.slides li' ).eq( slider.currentSlide ).find( 'iframe' );
				et_video_src 	= $et_video.attr( 'src' );

				// remove autoplay, so it doesn't play in background
				if ( -1 !== et_video_src.indexOf( 'autoplay=1' ) ) {
					et_video_src = et_video_src.replace( /autoplay=1/g, '' );
					$et_video.addClass( 'et_autoplay_removed' );
				}

				$et_video.attr( 'src', '' );
				$et_video.attr( 'src', et_video_src );
			}

			// bring autoplay back to the video it was attached to before
			if ( slider.find( '.slides li' ).eq( slider.animatingTo ).has( 'iframe' ) ) {
				$et_video 		= slider.find( '.slides li' ).eq( slider.animatingTo ).find( 'iframe' );
				et_video_src 	= $et_video.attr( 'src' );

				if ( $et_video.hasClass( 'et_autoplay_removed' ) ) $et_video.attr( 'src', et_video_src + '&autoplay=1' ).removeClass( 'et_autoplay_removed' );
			}
		}
	});

	$(window).load( function(){
		et_columns_height_fix();

		$('span.menu_slide').css('display','block');

		$top_menu.css( 'visibility', 'visible' );
		$second_menu.css( 'visibility', 'visible' );
	} );

	function et_columns_height_fix(){
		var footer_widget_min_height = 0,
			$footer_widget = $('.footer-widget'),
			featured_tab_min_height = 0,
			$featured_tab = $('#switcher .switcher-content');

		$footer_widget.css( 'minHeight', 0 );
		$featured_tab.css( 'minHeight', 0 );

		if ( et_container_width < 440 ) return;

		$footer_widget.each( function(){
			var this_height = $(this).height();

			if ( footer_widget_min_height < this_height ) footer_widget_min_height = this_height;
		} ).each( function(){
			$(this).css( 'minHeight', footer_widget_min_height );
		} );

		$featured_tab.each( function(){
			var this_height = $(this).height();

			if ( featured_tab_min_height < this_height ) featured_tab_min_height = this_height;
		} ).each( function(){
			$(this).css( 'minHeight', featured_tab_min_height );
		} );
	}

	$(window).resize( function(){
		if ( et_container_width != $('#main-area .container').innerWidth() ){
			et_container_width = $('#main-area .container').innerWidth();
			et_columns_height_fix();
			if ( ! $featured.is(':visible') ) $featured.flexslider( 'pause' );
		}
	} );
})(jQuery)