<?php

/**
 * @file
 * Themeable functions for Google Site Search.
 */


/**
 * Process variables for gss-result.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - $result
 * - $module
 *
 * @see gss-result.tpl.php
 */
function template_preprocess_gss_result(&$variables) {
  $result = $variables['result'];
  $variables['url'] = check_url($result['link']);
  $variables['title'] = $result['title'];
  // Check for existence. User search does not include snippets.
  $variables['snippet'] = isset($result['snippet']) ? $result['snippet'] : '';
}


/**
 * Process variables for gss-results.tpl.php.
 *
 * The $variables array contains the following arguments:
 * - 
 * - 
 *
 * @see gss-results.tpl.php
 */
function template_preprocess_gss_results(&$variables) {
  $results = $variables['results'];
  $variables['head'] = $results['head'];
  unset($results['head']);
  $variables['pager'] = $results['pager'];
  unset($results['pager']);
  
  foreach ($results as $entry) {
    $variables['search_results'] .= theme('gss_result', $entry);
  }
}