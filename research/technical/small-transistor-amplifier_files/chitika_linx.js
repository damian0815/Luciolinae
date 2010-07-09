// Last modified: 2007-12-10
// Axiak: Cleaned it up.
//---------------------------------------------------


/* Parameters for Chitika|Linx. */
var ch_client, // The Chitika Username
    ch_sid, // The Chitika channel
    ch_non_contextual, // Whether or not to hit the server for keywords.
    ch_includeClasses, // Classes to include in spidering.
    ch_excludeClasses, // Classes to exclude in spidering.
    ch_linkStyle, // The type of underline. ['style3','style2']
    ch_linkColor, // The color of the link. (e.g. 'blue')
    ch_font, // The font of the link information.
    ch_borderColor, // The color of the border.
    ch_adCustomStyle, // Custom style information.
    ch_linkCustomStyle, // Custom style information for the link itself.
    ch_maxLinx, // The maximum number of links.
    ch_hideTime, // The time before it gets hidden.
    ch_keywords, // The keywords to include.
    ch_default_category, // The default category.
    ch_filter_category, // The category to filter with.
    ch_target, // The link target.
    ch_width, // The width of the window.
    ch_height, // The height of the window.
    ch_purl, // The URL of the page to Spider.
    chl_non_contextual, // Whether or not to hit the server for keywords.
    chl_linkStyle, // The style of the link. (e.g. 'blue')
    chl_linkColor, // The style of the underline ['style3','style2']
    chl_maxLinx, // The maximum number of links to highlight.
    chl_includeClasses, // The classes to include in spidering.
    chl_excludeClasses; // The classes to exclude in spidering.


window.$linx = {
  "noctxt":    0,
  "cur_id":    0,
  "num_linx":  0,
  "chl_path":  false,
  "cur_parent": false,
  "body_loaded": false, // Whether or not the body has loaded.
  "chl_linx":  "http://linx.chitika.net/linx?type=linx",
  "browser":   navigator.userAgent.toLowerCase(),
  "KEY_NODES": new Array(),
  "alphaset":  "abcdefghijklmnopqrstuvwxyzABCEFGHIJKLMNOPQRSTUVWXYZ0123456789",
  "K_FOUND":   0,
  "M_LINX":    1,
  "K_N_FOUND": 2,
  "IE":        0,
  "IE6":       0,
  "lTIE6":     0,
  "SAFARI":    0,
  "FF":        0,
  "OPERA":     0,
  "win98":     0,
  "bad_tags":  {},
  "settings":  {},
  "found_keywords": {},
  "timeout_id": "",
  "timeout_load_id": "",
  "highlighted": false,
  "post_highlight": function () {},


  "_": function (var_name, def) {
    /* Return the default if var_name is not defined. */
    return (typeof(window[var_name]) == "undefined") ? def: window[var_name];
  },


  "__": function (var_name) {
    /* Return the window variable with the given name if it"s defined. */
    return (typeof(window[var_name]) == "undefined") ? "":
            window[var_name];
   },


  "append_func": function (old, append) {
    /* Append the append function to the old function. */
    return function (e) {
      if (typeof(old) == "function") {
        old(e);
      }
      append(e);
    }
  },


  "attach_css": function () {
    /* Add the CSS html to the web page. */
    var head = document.getElementsByTagName("head")[0];
    var flyCss = document.createElement("link");
    flyCss.media = "all";
    flyCss.type = "text/css";
    flyCss.rel = "stylesheet";
    flyCss.href = this.get_path() + "linx.v3.css";
    head.appendChild(flyCss);	

    if (typeof(ch_alternateCss)!="undefined" && ch_alternateCss) {
      var altCss = document.createElement("link");
      altCss.media = "all";
      altCss.type = "text/css";
      altCss.rel = "stylesheet";
      altCss.href = ch_alternateCss;
      head.appendChild(altCss);
    }
    if (this.lTIE6 || this.win98) {
      var cssIE6 = document.createElement("link");
      cssIE6.media = "all";
      cssIE6.type = "text/css";
      cssIE6.rel = "stylesheet";
      cssIE6.href = this.get_path() + "linx.v3.lessThanIE6.css";
      head.appendChild(cssIE6);
    }
    if (this.IE6 && !this.win98) {
      var cssIE6 = document.createElement("link");
      cssIE6.media = "all";
      cssIE6.type = "text/css";
      cssIE6.rel = "stylesheet";
      cssIE6.href = this.get_path() + "linx.v3.IE6.css";
      head.appendChild(cssIE6);
    }
  },


  "attach_events": function () {
    /* Attach any events that need to "K_FOUND":   0,
  "M_LINX":    1,
  "K_N_FOUND": 2,be attached for Linx. */
    var w, t;
    w = window; t = this;
    w.onload = t.append_func(w.onload, t.win_sizes);
    w.onresize = t.append_func(w.onresize, t.win_sizes);
    w.onscroll = t.append_func(w.onscroll, t.win_sizes);
  },


  "build_bad_tags": function () {
    /* Build the bad tags object to reference in JS. */
    var ch_badtags = new Array(
    "script",
    "img",
    "input",
    "hr",
    "br",
    "head",
    "link",
    "param",
    "applet",
    "a",
    "object",
    "textarea",
    "h1",
    "select");
    this.bad_tags = new Object();
    for (var i=0; i < ch_badtags.length; i++) {
      this.bad_tags[ch_badtags[i]] = true;
    }
  },


  "build_linx_url": function () {
    /* Build the Linx URL to query the backend. */
    var purl, w, t;
    w = window;
    t = this;

    var purl = t.__("ch_purl");
    if (purl == "" && typeof(top.location.href) == "string")
      purl = top.location.href;

    if (purl == "" && typeof(location.href) == "string")
      purl = location.href;

    w.ch_purl = purl;
    w.ch_ref = document.referrer;

    /* Take care of the keywords. */
    w.ch_ikeywords = t.__("ch_keywords");
    if (typeof(w.ch_ikeywords) == "object" && ch_ikeywords["join"]) {
      /* The user used an array for the keywords. */
      w.ch_ikeywords = ch_ikeywords.join(",");
    }

    /* Set all the fields. */
    /* t.to_q(SERVER_NAME, VARIABLE_NAME, DEFAULT OR 0) */
    t.to_q("client", "ch_client", 0);
    t.to_q("sid", "ch_sid", 0);
    t.to_q("noctxt", "chl_non_contextual", "0");
    t.to_q("include_classes", "ch_includeClasses", 0);
    t.to_q("exclude_classes", "ch_excludeClasses", 0);
    t.to_q("include_classes", "chl_includeClasses", 0);
    t.to_q("exclude_classes", "chl_excludeClasses", 0);
    t.to_q("include_keywords", "ch_ikeywords", 0);
    t.to_q("exclude_keywords", "ch_excludeKeywords", 0);
    t.to_q("filtercat", "ch_filter_category", 0);
    t.to_q("defaultcat", "ch_default_category", 0);
    t.to_q("tptracker", "ch_third_party_tracker", 0);
    t.to_q("stealth_mode", "ch_stealth", 0);
    t.to_q("url", "ch_purl", 0);
    t.to_q("ref", "ch_ref", 0);
    t.to_q("max_num_links", "ch_maxLinx", 0);
  },


  "close_ad": function () {
    /* Close the currently displayed ad. */
    var ad = document.getElementById("chitika-linx-unit");
    if (ad) {
      ad.parentNode.removeChild(ad);
    }
    this.close_tab();
    this.cur_id = "";
  },


  "close_tab": function () {
    /* Close the current tab? */
    var obj = document.getElementById("chitika-linx-link");
    if (obj) {
      obj.parentNode.removeChild(obj);
    }
  },


  "compile_ads": function () {
    /* Stuff to do once we get ads. */
    var t = window.$linx;
    if (t.highlighted) {
      t.compile_ads_hl();
    } else {
      t.post_highlight = t.compile_ads_hl;
    }
  },


  "compile_ads_hl": function () {
    /* Stuff to do after the words are highlighted and we have ads. */
    var node, key, text, url;
    for (var i=0; i < this.KEY_NODES.length; i++) {
      node = this.KEY_NODES[i];
      if (!node) {
        continue;
      }
      key = node.innerHTML;
      if (!window.keyword_rpu[key.toLowerCase()]) {
        text = document.createTextNode(key);
        node.parentNode.replaceChild(text, node);
      } else {
        /* Set the Href so that we can go directly. */
        url = window.keyword_rpu[key.toLowerCase()][0]["durl"] + escape(ch_client);
        if (typeof(url) == "string" && url != "") {
          node.href = url;
          node.onclick = function () { return true; };
        }
      }
    }
    return;
  },


  "create_ad_html": function (id, keyword) {
    /* Generate the HTML for the Ad unit. */
    if (!this._("keyword_rpu", false)) {
      return false;
    }
    var cntArray = keyword_rpu[keyword.toLowerCase()];
    if (typeof(cntArray) != "object") {
      return false;
    }
    if (typeof(cntArray[0]) != "object") {
      return false;
    }
    cntArray = cntArray[0];

    var ad, description, title, product_link, h1, image, cnt_height, margin_top, info_class, content,
        vendor_title, height;

    ad = document.createElement("div");
    ad.className = "ch-linx-ad";
    ad.id = "chitika-linx-unit";

    if(this.settings.font) {
      ad.style.fontFamily = this.settings.font;
    }

    description = cntArray["ddesc"];
    image = cntArray["image"];
    vendor_title = 'http://chitika.com/search'; //cntArray["vendor_title"];
    title = cntArray["dtitle"];
    product_link = cntArray["durl"] +  'demo1';
    if (image == "http://scripts.chitika.net/eminimalls/images/1x1.gif") {
      image = false;
    }
    h1 = "See More at " + vendor_title;

    height = this.settings.height;
    if (!description) {
      if (image)
        height = 140;
      else
        height = 110;
    }

    if (!description && image) {
      cnt_height = "70px";
      margin_top = "20px";
    } else {
      margin_top = "0";
      cnt_height = "auto";
    }

    //info_class = image ? "ch-info" : "ch-info-nopadding";
	info_class = "ch-info";
    content = "";

    content += "<div class=\"ch-t\">";
    content += "<a href=\"javascript: $linx.close_ad();\" title=\"Close\" class=\"ch-close\">[X]</a>";
    content += "<a href=\"http://chitika.com/linx_overview.php?refid="+escape(ch_client)+
               "\" title=\"Linx Overview\" target=\"_blank\" class=\"ch-quest\">[?]</a>";
    content += "<span class=\"ch-brand\">[Chitika Linx]</span>";
    content += "<div>Related Product</div>";
    content += "</div>";
    content += "<div class=\"ch-cnt\">";
    content += "<div class=\"ch-c\" id=\"ch-content\" style=\"height:" + cnt_height + "\">";
	content += "<div class=\"ch-img\"><img src=\"http://scripts.chitika.net/screenshots/thumbnail.php?q=" + keyword + "\" /></div>";
   // if (image) {
   //   content += "<div class=\"ch-img\"><img src=\"" + image + "\" width=\"80\" /></div>";
   // }
    content += "<div class=\"" + info_class + "\">";
    content += "<div class=\"ch-h2\">Interested in <b>" + keyword + "</b>?</div>";
    if (description) {
      content += "<div class=\"ch-p\" style=\"font-size:11px;\">Find more information about " + keyword+ " now.</div>";
    }
    content += "<div class=\"ch-g\">" + vendor_title + "</div>";
    content += "</div>";
    content += "</div>";
    content += "</div><div class=\"ch-b\">&nbsp;</div>" + 
               "<a href=\"http://mm.chitika.net/search?q=" + keyword + "&linx=1&sid=" + escape(ch_sid) + "&client=" + escape(ch_client) + "\" class=\"ch-sneeze\" target=\"" + this.settings.target +
               "\">&nbsp;</a>";

    ad.innerHTML = content;
    return ad;
  },


  "create_elem": function (tagName, className) {
    /* Create an element by a given tagname and class name. */
    var x = document.createElement(tagName);
    x.className = className;
    x.innerHTML = "&nbsp;";
    return x;
  },

  "create_link_html": function (keyword, adIsBottom) {
    /* Create the HTML for the link itself. */
    var hover_img, cur_hover_img, span, sp1, sp2, sp3, strong, s;
    s = this.settings;
    hover_img = document.createElement("img");

    cur_hover_url = this.hover_url + "&keyword=" + escape(keyword);

    hover_img.src = cur_hover_url;
    hover_img.style.display = "none";

    span = document.createElement("span");
    sp1 = this.create_elem("b", "lx-sp1");
    sp2 = this.create_elem("b", "lx-sp2"); 
    sp3 = this.create_elem("b", "lx-sp3");
    strong = document.createElement("strong");
    strong.innerHTML = keyword;

    if (s.adCustomStyle) {
      sp1.style.background=ch_borderColor;
      sp2.style.cssText = sp3.style.cssText = strong.style.cssText = s.adCustomStyle;
    }

    if (s.lxTextColor) {
      span.style.color = strong.style.color = s.lxTextColor;
    }

    if (s.font) {
      span.style.fontFamily = s.font;
    }

    if (adIsBottom) {
      span.appendChild(strong);
      span.appendChild(sp3);
      span.appendChild(sp2);
      span.appendChild(sp1);
    } else {
      span.appendChild(sp1);
      span.appendChild(sp2);
      span.appendChild(sp3);
      span.appendChild(strong);
    }
    return span;
  },


  "find_keyword": function (textNode, keyword) {
    /* Find a keyword and highlight it. */
    var linkNode, spanNode, startNode, endNode, keywordNode, currentFlyout, textString, parent = textNode.parentNode;
    var keywordLC, textStringLower, start, symbolBefore, symbolAfter, startPiece, endPiece, actualKeyword, currentKeyword, cur_id;

    cur_id = this.num_linx + 1;

    if (textNode.tagName == "LABEL") {
      textString = textNode.innerHTML;
    } else {
      textString = textNode.data;
    }
    if (typeof(textString) == "undefined") {
      return this.K_N_FOUND;
    }

    keywordLC = String(keyword).toLowerCase();

    textStringLower = textString.toLowerCase();
    start = textStringLower.indexOf(keywordLC, (parent==this.cur_parent ? 20 : 0));

    if (start == -1) {
      return this.K_N_FOUND;
    }

    symbolBefore = (start == 0) ? " " : textString.substring(start-1, start);
    symbolAfter = (textString.length - start - keyword.length == 0) ? " " :
                  textString.substring(start + keyword.length, start + keyword.length+1);
    startPiece = textString.substring(0, start);
    endPiece = textString.substring(start + keyword.length, textString.length);
    actualKeyword = textString.substring(start, keyword.length + start);
    startNode = document.createTextNode(startPiece);
    keywordNode = document.createTextNode(actualKeyword);
    endNode = document.createTextNode(endPiece);
    linkNode = document.createElement("a");

    if (this.alphaset.indexOf(symbolBefore) == -1 && 
        this.alphaset.indexOf(symbolAfter) == -1) {
      currentKeyword = keyword;
      if (symbolAfter == "s") {
        keyword += "s";
      }
      currentFlyout = "lx" + cur_id;

      linkNode.rel = currentKeyword.toLowerCase();
      linkNode.href = "";
      linkNode.target = this.settings.target;
      linkNode.id = currentFlyout;
      linkNode.appendChild(keywordNode);
      linkNode.className = "lx-link-" + this.settings.linkStyle;
      linkNode.style.cssText = this.settings.linkCustomStyle;
      linkNode.i = cur_id;

      linkNode.onclick = function() {
        if (typeof(window.keyword_rpu) == "undefined") {
          return false;
        }
        this.href = window.keyword_rpu[this.rel][0]["durl"]
                                                  + "demo4";
        return true;
      };

      linkNode.onmouseover = function() {
        window.status = "";
        window.$linx.show_ad(this.i, currentKeyword);
        window.clearTimeout(window.$linx.timeout_id);
      };

      linkNode.onmouseout = function() {
        window.status = "";
        window.$linx.timeout_id = setTimeout(function () { window.$linx.close_ad(); },
                                             window.$linx.settings.hideTime);
      };

      this.KEY_NODES.push(linkNode);

      parent.insertBefore(startNode, textNode);
      parent.insertBefore(linkNode, textNode);
      parent.insertBefore(endNode, textNode);
      parent.removeChild(textNode);

      textString = endPiece;
      textNode = endNode;

      this.cur_parent = parent;
      this.cur_id ++;
      this.num_linx ++;

      if (this.settings.maxLinx > 0 && this.num_linx >= this.settings.maxLinx) {
        return this.M_LINX;
      } else {
        return this.K_FOUND;
      }
    }
    return this.K_N_FOUND;
  },


  "findPosX": function (obj) {
    /* Find the X position of an element. */
    var curleft = 0;
    if (obj.offsetParent) {
      while (1) {
        curleft += obj.offsetLeft;
        if(!obj.offsetParent) {
          break;
        }
        obj = obj.offsetParent;
      }
    } else if (obj.x) {
        curleft += obj.x;
    }
    return curleft;
  },


  "findPosY": function (obj) {
    /* Find the Y position of an object. */
    var curtop = 0;
    if (obj.offsetParent) {
      while (1) {
        curtop += obj.offsetTop;
        if(!obj.offsetParent) {
          break;
        }
        obj = obj.offsetParent;
      }
    } else if (obj.y) {
      curtop += obj.y;
    }
    return curtop;
  },


  "init_settings": function () {
    /* Initialize the settings object with settings given by the JS. */
    var _ = this._;
    var s = {
      "hideTime":    _("ch_hideTime", 1500),
      "font":        _("ch_font", false),
      "borderColor": _("ch_borderColor", "#000"),
      "bgColor":     _("ch_bgColor", "#fff"),
      "maxLinx":     _("chl_maxLinx", _("ch_maxLinx", 7)),
      "linkColor":   _("chl_linkColor", _("ch_linkColor", "blue")),
      "linkStyle":   _("chl_linkStyle", _("ch_linkStyle", "style3")),
      "width":       _("chl_width", 374),
      "height":      _("chl_height", 150),
      "target":      _("ch_target", "_blank"),
      "lxTextColor": _("chl_lxTextColor", 0),
      "client":      _("ch_client", "demo"),
      "url":         _("ch_purl", false),
      "ref":         _("ch_ref", false),
      "sid":         _("ch_sid", false)
    };
    s.adCustomStyle = _("ch_adCustomStyle", "border-left-color: " + s.borderColor +
                                                   "; border-right-color: " + s.borderColor +
                                                   "; background: " + s.bgColor);
    s.linkCustomStyle = _("ch_linkCustomStyle", "border-bottom-color: " + s.linkColor + "; color: " + 
                                                       s.linkColor);

    if (s.hideTime <= 60)
      s.hideTime *= 1000;

    /* Build the HOVER URL. */
    this.hover_url = "http://linx.chitika.net/linx/hover.gif?client=" + escape(s.client);
    if (s.url)
      this.hover_url += "&url=" + escape(s.url);

    if (s.sid)
      this.hover_url += "&sid=" + escape(s.sid);

    if (s.ref)
      this.hover_url += "&ref=" + escape(s.ref);


    this.settings = s;
  },

  "gen_tab": function (id, keyword, adIsBottom) {
    /* Generate the tab. */
    if (typeof(keyword_rpu) == "undefined")
      return;

    var el = document.getElementById("lx"+id);
    var link = document.createElement("a");

    link.href="";
    link.ref = keyword.toLowerCase();

    link.onclick = function (e) {
        link.href = w.keyword_rpu[keyword.toLowerCase()][0]["durl"] + 'demo2';
        return true;
    }

    var span = this.create_link_html(keyword, adIsBottom);
    var w = el.offsetWidth;

    var keywordW = parseInt(keyword.length*10);
    w = keywordW>w ? keywordW : w;

    link.style.width = w+"px";
    link.className = "lx-link";
    link.id = "chitika-linx-link";
    link.appendChild(span);
    return link;
  },


  "get_browser_info": function () {
    /* Get the current browser. */
    var temp, version, browser;
    browser=navigator.userAgent.toLowerCase();
    if (browser.indexOf("msie") != -1) {
      this.IE = 1;
      temp=navigator.appVersion.split("MSIE");
      version=parseFloat(temp[1]);
      if(version < 7) { 
        if(version < 6) {
          this.lTIE6=1;
        } else {
          this.IE6=1;
        }
      }
    } else if (browser.indexOf("safari") != -1) {
      this.SAFARI = 1; 
    } else if(browser.indexOf("opera") != -1) {
      this.OPERA = 1;
    } else if(browser.indexOf("firefox") != -1) {
      this.FF = 1;
    }
    if (browser.indexOf("win98") != -1) {
      this.win98=1;
    }
  },


  "get_path": function () {
    /* Get the current path of this script. */
    this.chl_path = "http://scripts.chitika.net/static/linx/";
    if (this.chl_path) {
      return this.chl_path;
    }
    var script_name = "chitika_linx";
    var allSc = document.getElementsByTagName("script");
    var chl_path = "";

    for (var i = 0; i < allSc.length; i++) {
      if(allSc[i].src.indexOf(script_name) != -1) {
        chl_path = allSc[i].src.replace(/chitika_linx\.v3\.js(\?.*)?$/,"");
        break;
    }
    this.chl_path = chl_path;
    return chl_path;
    }
  },


  "parse_document": function (node, keyword, maxDepth, currentDepth) {
    /* Return one of: [K_FOUND: Keyword found, M_LINX: Maximum # of links, K_N_FOUND: Keyword not found. */
    if ((node.nodeType == 3) || (node.tagName == "LABEL")) {
      var findResult, keywords, cur_res, removed=0;
      keywords = keyword;
      keyword = new Array();
      findResult = this.K_N_FOUND;
      if (typeof(keywords) == "string")
        keywords = new Array(keywords);

      for (var i = 0; i < keywords.length; i++)
        keyword.push(keywords[i]);

      for (var i = 0; i < keywords.length; i++) {
        cur_res = this.find_keyword(node, keyword[i]);
        if (cur_res != this.K_N_FOUND) {
          this.found_keywords[keyword[i].toLowerCase()] = 1;
          findResult = cur_res;
          keyword.splice(i - removed, 1);
          removed ++;
          if (keyword.length == 0) {
            return cur_res;
          }
        }
      }
    }
    var children = node.childNodes;
    /* compare the depths so we don't go needlessly far */
    if (node.tagName && (this.bad_tags[node.tagName.toLowerCase()] != true) && maxDepth > currentDepth) {
      for (var i = 0; i < children.length; i++) {
        var findResult = this.parse_document(children[i], keyword, maxDepth, currentDepth + 1);
	if (findResult != this.K_N_FOUND) {
          return findResult;
        }
      }
    }
    return this.K_N_FOUND;
  },


  "post_body_load": function (e) {
    /* Since this is run on an event, we need to get $linx explicitly. */
    var l = window.$linx;
    l.attach_css();
    l.win_sizes();
    l.attach_events();
    l.swf_fix();
    if (l.noctxt)
      l.parse_keys();
    else
      l.spider_keys();

    l.highlighted = true;
    l.post_highlight();
  },


  "post_spider_init": function () {
    /* Stuff to run after spidering has completed. */
    if (this.body_loaded)
      this.post_body_load(0);
    else
      window.onload = this.append_func(window.onload, this.post_body_load);

    return;
  },


  "pre_spider_init": function () {
    /* Do any of the really early initializations. */
    var t = this;
    t.noctxt = (t.__("chl_non_contextual") >= 1) ? 1: 0;
    t.init_settings();
    t.get_browser_info();
    t.build_bad_tags();
    t.build_linx_url();

    /* Signal when the body has finished loading. */
    document.body.onload = t.append_func(document.body.onload, function (e) {
        window.$linx.body_loaded = true; });

    /* Call the Linx Server for information. */
    document.write("<" + "script type=\"text/javascript\" src=\"" + this.chl_linx + "\"><"+"/sc"+"ript>");
  },


  "show_ad": function (id, keyword) {
    /* Show an ad by a given Ad id and a keyword. */
    if (!(this._("keyword_rpu", false) && window.keyword_rpu[keyword.toLowerCase()])) {
      this.timeout_load_id = setTimeout(function () { window.$linx.show_ad(id, keyword); }, 100);
      return false;
    }
    var ad, getleft, gettop, width, keyW, adIsBottom, frameTop, tab,
        verticalOffset, tabLeft, offsetLeft, offsetRight, frameLeft;

    if (document.getElementById("chitika-linx-unit") && id != this.cur_id) {
      this.close_ad();
    }

    var lx = document.getElementById('lx'+id);

    clearTimeout(this.timeout_load_id);
    ad = this.create_ad_html(id, keyword);
    getleft = this.findPosX(lx);
    gettop = this.findPosY(lx);
    width = lx.offsetWidth;
    keyW = parseInt(keyword.length * 10);
    keyW = (keyW > width) ? keyW : width;
    adIsBottom = 0;
    verticalOffset = 10;
    if ((gettop - 30 - this.settings.height) < this.winHeight) {
      frameTop = gettop + 22 + verticalOffset;
    } else {
      frameTop = gettop - this.settings.height + 1;
      adIsBottom = 1;
    }

    tabLeft = (getleft + keyW > this.winWidth - 15)  ? getleft + width - keyW : getleft;
    offsetLeft = (getleft > 15) ? 15 : getleft - 1;
    offsetRight = (tabLeft + keyW < this.winWidth - 15) ? 15 : this.winWidth - tabLeft - keyW - 1;
    frameLeft = (getleft + this.settings.width > this.winWidth) ?
                tabLeft - this.settings.width + keyW + offsetRight : getleft - offsetLeft;

    ad.style.top = frameTop + "px";
    ad.style.left = (frameLeft + 20) + "px";
    ad.style.zIndex = "19000";
    ad.style.position= "absolute";


    if (id != this.cur_id && !document.getElementById("chitika-linx-unit")) {
      document.body.insertBefore(ad, document.body.firstChild);
      tab = this.gen_tab(id, keyword, adIsBottom);
      tab.style.top = (gettop) + "px";
      tab.style.left = (tabLeft) + "px";
      ad.onmouseout = tab.onmouseout = function() {
        window.$linx.timeout_id = setTimeout(function () { window.$linx.close_ad(); },
                                                          window.$linx.settings.hideTime)
        };

      ad.onmouseover = tab.onmouseover = function() {
        window.clearTimeout(window.$linx.timeout_id);
      };
    }

    this.cur_id = id;
    return;
  },


  "spider_keys": function () {
    /* Spider the DOM for the keywords in window.keywords. */
    var maxDepth, className, allClasses, keyword;
    if (typeof(window.keywords) != "object")
      return;

    for (var i in window.keywords) {
      keyword = keywords[i][0];
      className = keywords[i][1];

      if (typeof(keyword)=='undefined') {
        continue;
      }

      if (typeof(className)=='object') {
        maxDepth = className[1] + 3;
        className = className[0];
      } else {
        maxDepth = 100
      }

      if ((typeof(className) == 'number') && className==0) {
        allClasses = document.getElementsByTagName('body');
        maxDepth = 10000;
      } else {
        allClasses = getElementsByClassName(className);
      }

      for (var p=0; p < allClasses.length; p++) {
        var findResult = this.parse_document(allClasses[p], keyword, maxDepth, 0);
        if (findResult == this.M_LINX) {
          return this.M_LINX;
        }
        if (findResult == this.K_FOUND) {
          break;
        }
      }
    }
  },


  "swf_fix": function () {
    /* re-Position all Flash so it appears below Linx. */
    var allObjs = document.getElementsByTagName("object");

    for (var i = 0; i < allObjs.length; i++) {
      var oldFla = allObjs[i];
      var classId = oldFla.classid || "";
      if (classId.toLowerCase() != "clsid:d27cdb6e-ae6d-11cf-96b8-444553540000")
        continue;

      var parent = allObjs[i].parentNode;
      var newObj = document.createElement("span");
      var objStartTag = "<object id=\"" + oldFla.id + "\" " +
          " classid=\"clsid:d27cdb6e-ae6d-11cf-96b8-444553540000\"" +
          " width=\"" + oldFla.width + "\" height=\"" + oldFla.height + "\" style=\"" +
          oldFla.style.cssText +
          "\" codebase=\"" + oldFla.codeBase +"\">";
      var objEndTag = "</object>";
      var objInner = "";
      var allPars = oldFla.getElementsByTagName("param");
      var oHaveWmode = false;

      for (var j = 0; j < allPars.length; j++) {
        if (allPars[j].name.toLowerCase()=="wmode" && allPars[j].value.toLowerCase() == "transparent") {
          oHaveWmode = true;
          break;
        }
        objInner += "<param name=\"" + allPars[j].name + "\" value=\"" + allPars[j].value + "\" />";
      }
      objInner += "<param name=\"wmode\" value=\"transparent\" />";

      if(!oHaveWmode) {
        var newEmbedTag = "";
        var embedHaveWmode = false;
        if (!this.IE) {
          /* The current browser is NOT ie. */
          var embedTag = oldFla.getElementsByTagName("embed")[0];
          newEmbedTag += "<embed ";
          for (var p = 0; p < embedTag.attributes.length; p++) {
            var eName = embedTag.attributes[p].nodeName;
            var eValue = embedTag.attributes[p].nodeValue;
            newEmbedTag += eName + "=\"" + eValue + "\" ";
            if (eName.toLowerCase() == "wmode" && eValue.toLowerCase() == "transparent") {
              embedHaveWmode = true;
            }
          }
          if(!embedHaveWmode) {
            newEmbedTag += "wmode=\"transparent\" ";
          }
          newEmbedTag += " />";
        }
        newObj.innerHTML = objStartTag + objInner + newEmbedTag + objEndTag;
        parent.replaceChild(newObj,oldFla);
      }
    }
    if (!this.IE) {
      /* If the browser is not currently IE. */
      var allEmbeds = document.getElementsByTagName("embed");
      for (var i = 0; i < allEmbeds.length; i++) {
        var theEmbed = allEmbeds[i];
        var eAttributes = theEmbed.attributes;
        var haveWmode = false;
        var embedTag = "<embed ";
        for (var j = 0; j < eAttributes.length; j++) {
          var eName = eAttributes[j].nodeName.toLowerCase();
          var eValue = eAttributes[j].nodeValue;
          if (eName.toLowerCase() == "wmode" && eValue.toLowerCase() == "transparent") {
            haveWmode = true;
            break;
          }
          embedTag += eName + "=\"" + eValue + "\" ";
        }
        if (!haveWmode) {
          var tempObj = document.createElement("span");
          embedTag += " wmode=\"transparent\" />";
          tempObj.innerHTML = embedTag;
          var parent = theEmbed.parentNode;
          var newObj = parent.replaceChild(tempObj,theEmbed);
        }
      }
    }
  },


  "to_q": function (name, var_name, def) {
    /* Append the name and value to the Linx URL. */
    var value = this.__(var_name);
    if (!value) {
      if (!def)
        return;
      else
        value = def;
    }
    this.chl_linx += "&" + name + "=" + escape(value);
  },


  "win_sizes": function (e) {
    /* Calculate the size and position of the window. */
    var topPos, screenH, d, w, t;
    t = window.$linx;
    d = document;
    w = window;
    t.close_ad();
    topPos = 0;
    var screenH = 0;
    if (w.pageYOffset && w.innerWidth) {
      topPos = w.pageYOffset;
      screenH = w.innerHeight;
    } else if (d.dElement && d.dElement.scrollTop) {
      topPos = d.dElement.scrollTop;
      screenH = d.dElement.clientHeight;
    } else if (d.body) {
      topPos = d.body.scrollTop;
      screenH = d.body.clientHeight;
    }

    t.winWidth = d.body.offsetWidth;
    t.winHeight = topPos + screenH / 2;
  }

}

$linx.pre_spider_init();

window.chInit = function () {
  $linx.post_spider_init();
}

window.chAdsCompile = function () {
  $linx.compile_ads();
}

/* Generic Utilities
  ===================  */
function getElementsByClassName(strClass, strTag, objContElm) {
  strTag = strTag || "*";
  var retNodes = [];
  var class_regex = new RegExp("\\b" + strClass + "\\b", "i");
  var parent = objContElm || document;
  var elems = parent.getElementsByTagName(strTag);
  for (var i = 0; i < elems.length; i++) {
    if (class_regex.test(elems[i].className)) {
        retNodes.push(elems[i]);
    }
  }
  return retNodes;
}

/**
 These functions are useful, but not currently being used.
function dump(arr,level) {
  var dumped_text = "";
  if (!level) level = 0;

  //The padding given at the beginning of the line.
  var level_padding = "";
  for (var j=0;j<level+1;j++) level_padding += "    ";

  if (typeof(arr) == 'object') { //Array/Hashes/Objects
    for (var item in arr) {
      var value = arr[item];

      if(typeof(value) == 'object') { //If it is an array,
        dumped_text += level_padding + "'" + item + "' ...\n";
        dumped_text += dump(value,level+1);
      } else {
        dumped_text += level_padding + "'" + item + "' => \"" + value + "\"\n";
      }
    }
  } else { //Stings/Chars/Numbers etc.
    dumped_text = "===>"+arr+"<===("+typeof(arr)+")";
  }
  return dumped_text;
}

String.prototype.trim = function() {
  return this.replace(/^\s+|\s+$/g,"");
}

**/

Array.prototype.push = function(value) {
  this[this.length] = value;
}
