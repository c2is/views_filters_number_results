<?php
/**
 * @file
 * Set "views_handler_filter_term_node_tid_with_node_count" handler in views term reference exposed filter
 */

/**
 * Alter table structure to change term reference exposed filter handler
 */
function views_filters_number_results_views_data_alter(&$data) {

    // Search data with "views_handler_filter_term_node_tid" handler
    foreach ($data as $table => $config) {
        foreach ($config as $item => $item_config) {
            if (isset($item_config['filter']['handler'])
                && $item_config['filter']['handler'] == 'views_handler_filter_term_node_tid') {

                // Replace handler by "views_handler_filter_term_node_tid_with_node_count"
                $data[$table][$item]['filter']['handler'] = 'views_handler_filter_term_node_tid_with_node_count';

            }
        }
    }
}
