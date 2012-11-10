<?php

// $Id$

/**
 * Class for interaction with Google Site Search.
 */
class GoogleSiteSearch {

  /**
   * The Google key to use.
   * @var string
   */
  private $key = null;

  /**
   * The language to use in the search.
   * @var string
   */
  private $language = null;
  
  /**
   * The search query.
   * @var string
   */
  private $query = null;

  /**
   * Extra parameters to be passed onto Google CSE.
   * @var array
   */
  private $extraParams = null;

  /**
   * Page size, number of results per page.
   * @var int
   */
  private $pageSize = 20;

  /**
   * Pager size, number of pages to show in the pager.
   * @var int
   */
  private $pagerSize = 19;

  /**
   * Current page in the results.
   * @var int
   */
  private $currentPage = null;

  /**
   * Total results for a search.
   * @var int
   */
  private $totalResults = 0;

  /**
   * The search results return from Google.
   * @var SimpleXml
   */
  private $searchResultsXml = null;

  /**
   * Create a new instance of GoogleSiteSearch.
   *
   * @param string
   */
  public function __construct($query, $key = null, $pageSize = 20, $extraParams = null) {
    $this->query = $query;
    $this->key = $key;
    $this->pageSize = $pageSize;
    $this->extraParams = $extraParams;
  }

  /**
   * Set Google key.
   *
   * @param string $key The Google key.
   * @return void
   */
  public function SetKey($key) {
    $this->key = $key;
  }

  /**
   * Get Google key
   *
   * @return string The Google key.
   */
  public function GetKey() {
    return $this->key;
  }

  /**
   * Set the language.
   *
   * @param str $language The new language.
   * @return void
   */
  public function SetLanguage($language) {
    $this->language = $language;
  }
  
  /**
   * Set the page size.
   *
   * @param int $pageSize The new page size.
   * @return void
   */
  public function SetPageSize($pageSize) {
    $this->pageSize = $pageSize;
  }

  /**
   * Set the pager size.
   *
   * @param int $pagerSize The new pager size.
   * @return void
   */
  public function SetPagerSize($pagerSize) {
    $this->pagerSize = $pagerSize;
  }

  /**
   * Get current page size.
   *
   * @return int The current page size.
   */
  public function GetPageSize() {
    return $this->pageSize;
  }

  /**
   * Get current pager size.
   *
   * @return int The current pager size.
   */
  public function GetPagerSize() {
    return $this->pagerSize;
  }
  /**
   * Get current page in the results.
   *
   * @return int The current page.
   */
  public function GetCurrentPage() {
    return $this->currentPage;
  }

  /**
   * Get total results for current search.
   *
   * @return int Total number of results.
   */
  public function GetTotalResults() {
    return $this->totalResults;
  }

  /**
   * Get total number of pages for search.
   *
   * @return int Total number of pages.
   */
  public function GetTotalPages() {
    return ($this->totalResults / $this->pageSize + 1);
  }

  /**
   * Get query current search.
   *
   * @return string The current query.
   */
  public function GetQuery() {
    return $this->query;
  }

  /**
   * Get query extra parameters array.
   *
   * @return array The extra parameters to CSE.
   */
  public function GetExtraParameters() {
    return $this->extraParams;
  }

  /**
   * Get search results.
   *
   * @param string $query The search query.
   * @param int $page The page to get results from.
   * @return array The search results.
   * @throws Exception
   */
  public function GetSearchResults($page = 1) {
    // set page
    $this->currentPage = $page;

    // calculate start postition based on page
    $startPos = ($this->currentPage - 1) * $this->pageSize;
    
    if ($this->language) {
     $language = "&lr=lang_{$this->language}"; 
    }
    else {
      $language = null;
    }

    // prepare query parameters for URL assembly
    if (count($this->extraParams) > 0) {
      $extraParamsQuery = '&' . http_build_query($this->extraParams);
    }
    else {
      $extraParamsQuery = null;
    }

    // assemble url
    $url = "http://www.google.com/search?start={$startPos}&num={$this->pageSize}{$language}&client=google-csbe&output=xml_no_dtd&cx={$this->key}&q={$this->query}{$extraParamsQuery}";

    // fetch results from google
    $results = simplexml_load_file($url);
    
    if ($results !== false) {
      if (!isset($results->RES->M)) {
        // no results, return null
        return null;
      }
      else {
        // save total results
        $this->totalResults = intval((string) $results->RES->M);

        // init results array
        $arr = array();

        // loop results
        foreach ($results->RES->R as $result) {
          // init result array
          $item = array();
          $item['title'] = (string) $result->T;
          $item['url'] = (string) $result->UE;
          $item['description'] = (string) $result->S;
          $arr[] = $item;
        }
        
        // return results
        return $arr;
      }
    }
    else {
      throw new Exception('Could not load search results from Google.');
    }
  }

  /**
   * Get a pager for the search results.
   *
   * @return string The pager HTML code.
   */
  public function GetPager($link = '?page=') {

    // get total pages  
    $totalPages = $this->GetTotalPages();
    $currentPage = $this->GetCurrentPage();
    $pagerSize = $this->GetPagerSize();
    $halfPagerSize = $pagerSize / 2;
    
    if ($totalPages < 2) {
      return;
    }    
    
    $html = "<div class=\"item-list google-search-pager\"><ul class=\"pager\">\n";

    // Limit the number of pages to 50
    $totalPages = $totalPages > 50 ? 50 : $totalPages;
    
    if ($currentPage > 1) {
      $link_page = $link . ($currentPage - 1);
      $html .= "\t<li class=\"pager-previous\"><a href=\"{$link_page}\">" . t('‹ previous') . "</a></li>\n";
    }
    
    // Defines the start and finish of the pager
    $pagerStart = $currentPage - (round($pagerSize / 2) - 1);
    $pagerFinish = $currentPage + (round($pagerSize / 2) - 1);
    
    // Start values, if $pagerStart is negative
    if ($pagerStart < 1) {
      $pagerStart = 1;
      $pagerFinish = $pagerSize > $totalPages ? $totalPages : $pagerSize;
    }
    
    // Limit $pagerFinish to the same as $totalPages
    $pagerFinish = $pagerFinish > $totalPages ? $totalPages : $pagerFinish;
    
    if ($pagerStart > 1) {
      $html .= "\t<li class=\"pager-ellipsis\">...</li>\n";
    }
    
    for ($i = $pagerStart; $i <= $pagerFinish; $i++) {
      if ($i != $currentPage) {
        $link_page = $link . $i;
        $html .= "\t<li class=\"pager-item\"><a href=\"{$link_page}\">{$i}</a></li>\n";
      }
      else {
        $html .= "\t<li class=\"pager-current\">{$i}</li>\n";
      }
    }
    
    if ($pagerFinish < $totalPages) {
      $html .= "\t<li class=\"pager-ellipsis\">...</li>\n";
    }
    
    if ($currentPage < intval($totalPages)) {
      $link_page = $link . ($currentPage + 1);
      $html .= "\t<li class=\"pager-next\"><a href=\"{$link_page}\">" . t('next ›') . "</a></li>\n";
    }
    
    // close ol
    $html .= "</ul></div>\n";
    
    // return pager
    return $html;
  }

  
  /**
   * Get a head to the search results.
   *
   * @return string The search head HTML code.
   */
  public function GetSearchHead() {
    // get total pages  
    $totalResults = $this->GetTotalResults();
    $pageSize = $this->GetPageSize();
    $currentPage = $this->GetCurrentPage();
    $show_start = ($pageSize * $currentPage) - $pageSize + 1;
    $show_end = ($pageSize * $currentPage);

    if($show_end > $totalResults) {
      $show_end = $totalResults;
    }
    
    $html = '<div class="searchhead">';

    if($totalResults !== 0) {
      $html .= t('Shows') . ' ' . $show_start . ' ' . t('to') . ' ' . $show_end . ' ' . t('of') . ' ';
      $html .= t('approximately') . ' ' . $totalResults . ' ' . t('hits');
    }

    $html .= '</div>';
    
    // return search head
    return $html;
  }
  
  
}
